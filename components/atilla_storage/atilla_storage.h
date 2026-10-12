#pragma once
// ATILLA THE HOME — guarded SD SPI transport; read-only by default.
// Explicit opt-in pilot can issue limited writes after CSD capacity validation.
// Existing ESPHome SPIDevice instances handle CH422G EXIO4 chip select.
#include <cstdint>
#include <cinttypes>
#include <cstring>
#include <atomic>
#include <memory>
#include <new>
#include <utility>
#include "esphome/core/log.h"

namespace esphome::atilla_storage {
struct Bus {
  void *ptr{nullptr};
  uint8_t (*transfer)(void *, uint8_t){nullptr};
  void (*enable)(void *){nullptr};
  void (*disable)(void *){nullptr};
  uint8_t tx(uint8_t b) const { return transfer(ptr,b); }
  void select() const { enable(ptr); }
  void deselect() const { disable(ptr); }
};
template<typename T> Bus wrap(T *p) {
  Bus b; b.ptr=p;
  b.transfer=[](void *o,uint8_t v)->uint8_t{return static_cast<T*>(o)->transfer_byte(v);};
  b.enable=[](void *o){static_cast<T*>(o)->enable();};
  b.disable=[](void *o){static_cast<T*>(o)->disable();};
  return b;
}
class ReadOnlySD {
 public:
  template<typename T,typename U> void bind(T *card, U *idle) {
    card_=wrap(card); idle_=wrap(idle);
  }
  bool begin() {
    initialized_=false; capacity_verified_=false; writes_allowed_=false; card_sectors_=0;
    if (!card_.ptr || !idle_.ptr) return false;
    idle_.select();
    for (int i=0;i<12;i++) idle_.tx(0xff);
    idle_.deselect();
    card_.select();
    bool ok=false;
    do {
      if (command_(0,0,0x95)!=1) break;
      if (command_(8,0x1AA,0x87)!=1) break;
      uint8_t echo[4]; for(auto &v:echo)v=card_.tx(0xff);
      if (echo[2]!=1 || echo[3]!=0xAA) break;
      bool ready=false;
      for(int i=0;i<200;i++) {
        uint8_t r55=command_(55,0,0x01);
        if(r55>1) break;
        uint8_t r41=command_(41,0x40000000,0x01);
        if(r41==0){ready=true;break;}
        if(r41!=1)break;
      }
      if(!ready || command_(58,0,0x01)!=0)break;
      uint8_t ocr[4];for(auto &v:ocr)v=card_.tx(0xff);
      if(!(ocr[0]&0x40))break; // SDHC/SDXC block addressed
      uint8_t sec[512];
      if(!read_selected_(0,sec) || !signature_(sec))break;
      uint8_t type=sec[450];
      if(type!=0x0B && type!=0x0C)break;
      partition_start_=le32_(sec+454);
      partition_sectors_=le32_(sec+458);
      if(!partition_start_ || !partition_sectors_)break;
      if(uint64_t(partition_start_)+partition_sectors_>0xFFFFFFFFULL)break;
      if(!read_selected_(partition_start_,sec)||!signature_(sec))break;
      if(sec[11]!=0 || sec[12]!=2 || !sec[13] || !sec[16])break;
      if(le32_(sec+36)==0 || le32_(sec+44)<2)break;
      ok=true;
    } while(false);
    card_.deselect();
    initialized_=ok;
    ESP_LOGI("atilla_storage","SDHC/FAT32 read-only init: %s",ok?"PASS":"FAIL");
    return ok;
  }
  // Physical absolute-LBA sector read, NOT filesystem-relative.
  bool read_sector(uint32_t lba,uint8_t *out) {
    if(!initialized_ || !out || (uint64_t(lba) >= uint64_t(partition_start_) + partition_sectors_))return false;
    card_.select();bool ok=read_selected_(lba,out);card_.deselect();return ok;
  }
  // Physical-card reads for FatFs: the MBR at LBA 0 must be accessible.
  bool read_physical_sector(uint32_t lba, uint8_t *out) {
    if (!initialized_ || !out || uint64_t(lba) >= physical_sector_limit()) return false;
    card_.select();
    bool ok = read_selected_(lba, out);
    card_.deselect();
    return ok;
  }
  uint64_t physical_sector_limit() const {
    return uint64_t(partition_start_) + partition_sectors_;
  }
  // SDHC CSD v2 card capacity: verified independently of FAT partition fields.
  bool verify_physical_capacity() {
    if (!initialized_) return false;
    uint8_t csd[16] = {};
    card_.select();
    bool ok = command_(9, 0, 0x01) == 0;
    if (ok) {
      uint8_t token = 0xff;
      for (int i=0;i<16000;i++) {
        token=card_.tx(0xff);
        if(token!=0xff)break;
      }
      ok=(token==0xfe);
      if(ok) {
        for(auto &b : csd)b=card_.tx(0xff);
        card_.tx(0xff);card_.tx(0xff);
      }
    }
    card_.deselect();
    if(!ok || (csd[0] >> 6)!=1) return false;
    const uint32_t c_size = (uint32_t(csd[7] & 0x3f) << 16) |
                            (uint32_t(csd[8]) << 8) | csd[9];
    card_sectors_ = (uint64_t(c_size)+1ULL)*1024ULL;
    capacity_verified_ = (physical_sector_limit() <= card_sectors_) &&
                         card_sectors_ <= uint64_t(UINT32_MAX);
    ESP_LOGI("atilla_storage", "SD CSD verified capacity sectors=%" PRIu64
             " partition_end=%" PRIu64 " status=%s", card_sectors_,
             physical_sector_limit(), capacity_verified_ ? "PASS" : "FAIL");
    return capacity_verified_;
  }
  bool enable_test_writes() {
    writes_allowed_ = initialized_ && capacity_verified_;
    return writes_allowed_;
  }
  void disable_test_writes() { writes_allowed_ = false; }
  bool write_physical_sector(uint32_t lba, const uint8_t *in) {
    if (!writes_allowed_ || !initialized_ || !capacity_verified_ || !in ||
        uint64_t(lba)<partition_start_ || uint64_t(lba)>=physical_sector_limit() ||
        uint64_t(lba)>=card_sectors_) return false;
    card_.select();
    bool ok=command_(24,lba,0x01)==0;
    if(ok) {
      card_.tx(0xff); card_.tx(0xfe);
      for(unsigned i=0;i<512;i++) card_.tx(in[i]);
      card_.tx(0xff);card_.tx(0xff);
      uint8_t response=0xff;
      for(unsigned attempt=0;attempt<16;attempt++) {
        response=card_.tx(0xff);
        if((response & 0x11)==0x01) break;
      }
      ok=(response & 0x1f)==0x05;
      if(ok) {
        bool ready=false;
        for(unsigned i=0;i<200000;i++) if(card_.tx(0xff)==0xff){ready=true;break;}
        ok=ready;
      }
    }
    card_.deselect();
    return ok;
  }
  uint32_t partition_start() const {return partition_start_;}
  uint32_t partition_sectors() const {return partition_sectors_;}
  bool initialized() const {return initialized_;}
  // Sector writes require both an explicit opt-in and verified card capacity.
 private:
  Bus card_,idle_;
  bool initialized_{false};
  bool capacity_verified_{false};
  bool writes_allowed_{false};
  uint64_t card_sectors_{0};
  uint32_t partition_start_{0},partition_sectors_{0};
  static bool signature_(const uint8_t *p) {return p[510]==0x55 && p[511]==0xAA;}
  static uint32_t le32_(const uint8_t *p) {
    return uint32_t(p[0]) | uint32_t(p[1])<<8 | uint32_t(p[2])<<16 | uint32_t(p[3])<<24;
  }
  uint8_t command_(uint8_t cmd,uint32_t arg,uint8_t crc) {
    card_.tx(0xff);card_.tx(uint8_t(0x40|cmd));
    for(int s=24;s>=0;s-=8)card_.tx(uint8_t(arg>>s));
    card_.tx(crc);
    for(int i=0;i<16;i++){uint8_t r=card_.tx(0xff);if((r&0x80)==0)return r;}
    return 0xff;
  }
  bool read_selected_(uint32_t lba,uint8_t *buf) {
    if(command_(17,lba,0x01)!=0)return false;
    uint8_t token=0xff;
    for(int i=0;i<16000;i++){token=card_.tx(0xff);if(token!=0xff)break;}
    if(token!=0xfe)return false;
    for(int i=0;i<512;i++)buf[i]=card_.tx(0xff);
    card_.tx(0xff);card_.tx(0xff); // card-supplied CRC16 currently ignored
    return true;
  }
};
} // namespace esphome::atilla_storage

#include "fatfs_readonly.h"
#include "graphics_https_fetch.h"
// ESPHome codegen integration: one delayed storage probe per boot.
// Experimental storage-service mode uses a separate FreeRTOS worker stack.
#include "esphome/core/component.h"
#include "esphome/components/spi_device/spi_device.h"
#include "esphome/core/hal.h"
namespace esphome::atilla_storage {
class AtillaStorage : public Component {
 public:
  void set_devices(spi_device::SPIDeviceComponent *card, spi_device::SPIDeviceComponent *idle) {
    card_ = card;
    idle_ = idle;
  }
  void set_fatfs_mount_test(bool enabled) { fatfs_mount_test_ = enabled; }
  void set_fatfs_file_read_test(bool enabled) { fatfs_file_read_test_ = enabled; }
  void set_fatfs_entry_probe(bool enabled) { fatfs_entry_probe_ = enabled; }
  void set_fatfs_fil_heap_probe(bool enabled) { fatfs_fil_heap_probe_ = enabled; }
  void set_fatfs_open_only_probe(bool enabled) { fatfs_open_only_probe_ = enabled; }
  void set_fatfs_batch_read_probe(bool enabled) { fatfs_batch_read_probe_ = enabled; }
  void set_asset_discovery_probe(bool enabled) { asset_discovery_probe_ = enabled; }
  void set_bounded_crc_probe(bool enabled) { bounded_crc_probe_ = enabled; }
  void set_combined_readonly_probe(bool enabled) { combined_readonly_probe_ = enabled; }
  void set_manifest_readonly_probe(bool enabled) { manifest_readonly_probe_ = enabled; }
  void set_cache_write_pilot(bool enabled) { cache_write_pilot_ = enabled; }
  void set_https_cache_pilot(bool enabled) { https_cache_pilot_ = enabled; }
  void set_cache_read_pilot(bool enabled) { cache_read_pilot_ = enabled; }
  void set_fatfs_write_roundtrip_test(bool enabled) { fatfs_write_roundtrip_test_ = enabled; }
  void set_storage_service_test(bool enabled) { storage_service_test_ = enabled; }
  void set_storage_repeat_test(bool enabled) { storage_repeat_test_ = enabled; }
  void set_settings_persistence_probe(bool enabled) { settings_persistence_probe_ = enabled; }
  void setup() override {
    start_ms_ = millis();
    ESP_LOGI("atilla_storage", "ATILLA SD read-only probe scheduled for +30 seconds");
  }
  void loop() override {
    // This is the only consumer of the worker-owned frame pointer. Image bytes
    // are never accessed by LVGL/ESPHome while FatFs has the SD mounted.
    if (cache_read_pilot_) {
      std::unique_ptr<experimental::ReadOnlyFatFs::CachedPng> ready(
          cache_png_pending_.exchange(nullptr, std::memory_order_acq_rel));
      if (ready && ready->data) {
        cache_image::Metadata verified{};
        const bool good = cache_image::inspect_verified_png(
            ready->data.get(), ready->meta.bytes, ready->meta.crc32, verified);
        ESP_LOGI("atilla_storage",
                 "CACHE MAIN-LOOP HANDOFF %s bytes=%" PRIu32
                 " dimensions=%ux%u NO WRITES (not displayed)",
                 good ? "PASS" : "FAIL", ready->meta.bytes,
                 ready->meta.width, ready->meta.height);
        // Main-loop ownership expires here. A future LVGL widget must instead
        // retain its owned decoded image buffer until the widget is destroyed.
      }
    }
    if (attempted_ || (uint32_t) (millis() - start_ms_) < 30000UL) return;
    attempted_ = true; // never repeat after a failure
    if (storage_service_test_ || storage_repeat_test_ || settings_persistence_probe_ || fatfs_batch_read_probe_ || asset_discovery_probe_ || bounded_crc_probe_ || combined_readonly_probe_ || manifest_readonly_probe_ || cache_write_pilot_ || https_cache_pilot_ || cache_read_pilot_) {
      ESP_LOGI("atilla_storage", "Starting one-shot storage worker with 24576-byte stack");
      const BaseType_t created = xTaskCreatePinnedToCore(
          &AtillaStorage::storage_worker_, "atilla_sd_worker",
          24576, this, 1, nullptr, 1);
      if (created != pdPASS)
        ESP_LOGE("atilla_storage", "Could not create SD worker; no storage test run");
      return;
    }
    run_probe_();
  }
  static void storage_worker_(void *parameter) {
    auto *self = static_cast<AtillaStorage *>(parameter);
    ESP_LOGI("atilla_storage", "SD worker started; stack minimum-free=%u bytes",
             unsigned(uxTaskGetStackHighWaterMark(nullptr)));
    self->run_probe_();
    ESP_LOGI("atilla_storage", "SD worker finished; stack minimum-free=%u bytes",
             unsigned(uxTaskGetStackHighWaterMark(nullptr)));
    vTaskDelete(nullptr);
  }
  void run_probe_() {
    if (card_ == nullptr || idle_ == nullptr) {
      ESP_LOGE("atilla_storage", "Read-only probe skipped: SPI device binding absent");
      return;
    }
    ReadOnlySD sd;
    sd.bind(card_, idle_);
    if (!sd.begin()) {
      ESP_LOGW("atilla_storage", "Read-only SD probe FAILED; no writes attempted");
      return;
    }
    uint8_t sector[512];
    if (!sd.read_sector(sd.partition_start(), sector)) {
      ESP_LOGW("atilla_storage", "FAT32 boot-sector read failed");
      return;
    }
    const uint16_t bytes_per_sector = uint16_t(sector[11]) | uint16_t(sector[12]) << 8;
    const uint8_t sectors_per_cluster = sector[13];
    ESP_LOGI("atilla_storage", "READ ONLY OK partition_start=%" PRIu32 " sectors=%" PRIu32
      " bytes_per_sector=%u sectors_per_cluster=%u", sd.partition_start(),
      sd.partition_sectors(), bytes_per_sector, sectors_per_cluster);
    // Bounded three-sector verification against the legacy YAML test. This is
    // raw FAT32 directory traversal, NOT a mounted filesystem or general fopen.
    const auto le16 = [](const uint8_t *p) -> uint16_t {
      return uint16_t(p[0]) | (uint16_t(p[1]) << 8);
    };
    const auto le32 = [](const uint8_t *p) -> uint32_t {
      return uint32_t(p[0]) | (uint32_t(p[1]) << 8) |
             (uint32_t(p[2]) << 16) | (uint32_t(p[3]) << 24);
    };
    const uint16_t reserved = le16(sector + 14);
    const uint8_t fat_count = sector[16];
    const uint32_t fat_size = le32(sector + 36);
    const uint32_t root_cluster = le32(sector + 44);
    const uint64_t data_start = uint64_t(sd.partition_start()) + reserved +
                                uint64_t(fat_count) * fat_size;
    const uint64_t part_end = uint64_t(sd.partition_start()) + sd.partition_sectors();
    if (bytes_per_sector != 512 || sectors_per_cluster == 0 ||
        (sectors_per_cluster & (sectors_per_cluster - 1)) ||
        fat_count == 0 || reserved == 0 || fat_size == 0 ||
        root_cluster < 2 || data_start >= part_end) {
      ESP_LOGW("atilla_storage", "FILE CHECK skipped: invalid FAT32 geometry");
      return;
    }
    const auto cluster_lba = [&](uint32_t cluster) -> uint64_t {
      return data_start + uint64_t(cluster - 2) * sectors_per_cluster;
    };
    const auto find_entry = [&](const uint8_t *buf, const char name[11],
                                bool directory, uint32_t &cluster, uint32_t &size) -> bool {
      for (int i = 0; i < 512; i += 32) {
        const uint8_t *e = buf + i;
        if (e[0] == 0x00) break;
        if (e[0] == 0xE5 || e[11] == 0x0F || (e[11] & 0x08)) continue;
        if ((bool(e[11] & 0x10)) != directory) continue;
        if (memcmp(e, name, 11) != 0) continue;
        cluster = (uint32_t(le16(e + 20)) << 16) | le16(e + 26);
        size = le32(e + 28);
        return cluster >= 2;
      }
      return false;
    };
    uint32_t dir_cluster = 0, ignored_size = 0;
    const uint64_t root_lba = cluster_lba(root_cluster);
    if (root_lba >= part_end || !sd.read_sector(uint32_t(root_lba), sector) ||
        !find_entry(sector, "SYSTEM~1   ", true, dir_cluster, ignored_size)) {
      ESP_LOGW("atilla_storage", "FILE CHECK: expected root directory not found");
      return;
    }
    uint32_t file_cluster = 0, file_size = 0;
    const uint64_t dir_lba = cluster_lba(dir_cluster);
    if (dir_lba >= part_end || !sd.read_sector(uint32_t(dir_lba), sector) ||
        !find_entry(sector, "WPSETT~1DAT", false, file_cluster, file_size)) {
      ESP_LOGW("atilla_storage", "FILE CHECK: expected 8.3 file not found");
      return;
    }
    const uint64_t file_lba = cluster_lba(file_cluster);
    if (file_size != 12 || file_lba >= part_end ||
        !sd.read_sector(uint32_t(file_lba), sector)) {
      ESP_LOGW("atilla_storage", "FILE CHECK: invalid file length or sector");
      return;
    }
    uint32_t crc = 0xFFFFFFFFu;
    for (uint32_t i = 0; i < file_size; i++) {
      crc ^= sector[i];
      for (int bit = 0; bit < 8; bit++)
        crc = (crc >> 1) ^ ((crc & 1u) ? 0xEDB88320u : 0u);
    }
    crc ^= 0xFFFFFFFFu;
    if (crc == 0x630105D1u)
      ESP_LOGI("atilla_storage", "FILE CONTENT READ PASSED: WPSETT~1.DAT bytes=12 CRC32=%08" PRIX32, crc);
    else
      ESP_LOGW("atilla_storage", "FILE CHECK CRC MISMATCH: got=%08" PRIX32 " expected=630105D1", crc);
    if (fatfs_mount_test_) this->run_fatfs_test_(sd);
  }
  void run_fatfs_test_(ReadOnlySD &sd) {
    graphics_https::Download verified_download;
    experimental::ReadOnlyFatFs::CachedPng verified_cache_png;
    if (https_cache_pilot_ && !graphics_https::download_public_test_asset(verified_download)) {
      ESP_LOGW("atilla_storage", "HTTPS CACHE: refusing SD writes because download failed");
      return;
    }
    ESP_LOGI("atilla_storage", "Opt-in FATFS mount starting (writes only in explicit pilot mode)");
    experimental::ReadOnlyFatFs fatfs(sd);
    if (!fatfs.mount()) {
      ESP_LOGW("atilla_storage", "FATFS MOUNT FAILED; no filesystem modifications attempted");
      return;
    }
    ESP_LOGI("atilla_storage", "FATFS READ-ONLY MOUNT PASS");
    // CRASH ISOLATION: the previous on-device run disconnected immediately
    // after FATFS mount PASS. Never attempt f_open/f_read without explicit opt-in.
    if (cache_read_pilot_) {
      const bool ok = fatfs.load_cache_png_readonly(1, 70, 0xF60D2E27U,
                                                    verified_cache_png);
      ESP_LOGI("atilla_storage", "CACHE PSRAM LOAD OVERALL %s NO WRITES",
               ok ? "PASS" : "FAIL");
    } else if (https_cache_pilot_) {
      const bool ok = fatfs.store_cache_png_once(
          graphics_fetch::kCacheSlot, verified_download.bytes.get(),
          verified_download.size, graphics_fetch::kExpectedCrc32);
      ESP_LOGI("atilla_storage", "HTTPS CACHE COMBINED %s download=PASS card=%s",
               ok ? "PASS" : "FAIL", ok ? "PASS" : "FAIL");
    } else if (cache_write_pilot_) {
      // Explicit opt-in; never included in a read-only mode by accident.
      const bool ok = fatfs.run_cache_write_pilot();
      ESP_LOGI("atilla_storage", "CACHE WRITE PILOT OVERALL %s", ok ? "PASS" : "FAIL");
    } else if (settings_persistence_probe_) {
      const bool ok = fatfs.run_settings_persistence_probe();
      ESP_LOGI("atilla_storage", "ATILLA SETTINGS PERSISTENCE %s",ok?"PASS":"FAIL");
    } else if (storage_repeat_test_) {
      const bool ok = fatfs.run_repeated_write_test();
      ESP_LOGI("atilla_storage", "ATILLA STORAGE REPEAT %s",ok?"PASS":"FAIL");
    } else if (storage_service_test_) {
      const bool ok = fatfs.run_storage_service_test();
      ESP_LOGI("atilla_storage", "ATILLA STORAGE SERVICE 4KiB %s",ok?"PASS":"FAIL");
    } else if (fatfs_write_roundtrip_test_) {
      const bool ok = fatfs.run_batched_write_roundtrip();
      ESP_LOGI("atilla_storage", "FATFS WRITE ROUNDTRIP %s", ok ? "PASS" : "FAIL");
    } else if (manifest_readonly_probe_) {
      const bool inventory_ok = fatfs.run_asset_discovery_read_only();
      const bool crc_ok = inventory_ok && fatfs.run_bounded_file_crc_probe();
      const bool asset_ok = crc_ok && fatfs.run_asset_reader_acceptance_readonly();
      const bool manifest_ok = asset_ok && fatfs.verify_asset_manifest_readonly();
      ESP_LOGI("atilla_storage",
               "COMBINED MANIFEST READ-ONLY %s discovery=%s crc=%s asset=%s manifest=%s NO WRITES",
               manifest_ok ? "PASS" : "FAIL", inventory_ok ? "PASS" : "FAIL",
               crc_ok ? "PASS" : "FAIL", asset_ok ? "PASS" : "FAIL",
               manifest_ok ? "PASS" : "FAIL");
    } else if (combined_readonly_probe_) {
      const bool inventory_ok = fatfs.run_asset_discovery_read_only();
      const bool crc_ok = inventory_ok && fatfs.run_bounded_file_crc_probe();
      const bool asset_ok = crc_ok && fatfs.run_asset_reader_acceptance_readonly();
      ESP_LOGI("atilla_storage",
               "COMBINED READ-ONLY %s discovery=%s crc=%s asset=%s NO WRITES",
               (inventory_ok && crc_ok && asset_ok) ? "PASS" : "FAIL",
               inventory_ok ? "PASS" : "FAIL", crc_ok ? "PASS" : "FAIL",
               asset_ok ? "PASS" : "FAIL");
    } else if (bounded_crc_probe_) {
      const bool ok = fatfs.run_bounded_file_crc_probe();
      ESP_LOGI("atilla_storage", "FILE CRC READ-ONLY PROBE %s", ok ? "PASS" : "FAIL");
    } else if (asset_discovery_probe_) {
      const bool ok = fatfs.run_asset_discovery_read_only();
      ESP_LOGI("atilla_storage", "ASSET DISCOVERY READ-ONLY %s", ok ? "PASS" : "FAIL");
    } else if (fatfs_batch_read_probe_) {
      const bool ok = fatfs.run_batched_read_only_diagnostics();
      ESP_LOGI("atilla_storage", "FATFS BATCH READ-ONLY %s", ok ? "PASS" : "FAIL");
    } else if (fatfs_open_only_probe_) {
      const bool ok = fatfs.probe_existing_file_open_only();
      ESP_LOGI("atilla_storage", "FATFS OPEN-ONLY probe %s", ok ? "PASS" : "FAIL");
    } else if (fatfs_fil_heap_probe_) {
      const bool ok = fatfs.probe_file_object_allocation();
      ESP_LOGI("atilla_storage", "FATFS FIL heap probe %s", ok ? "PASS" : "FAIL");
    } else if (fatfs_entry_probe_) {
      // Crash-isolation stage: deliberately do not enter the read function.
      ESP_LOGI("atilla_storage", "FATFS ENTRY PROBE PASS: no f_open, no f_read, no file object");
    } else if (fatfs_file_read_test_) {
      ESP_LOGW("atilla_storage", "EXPERIMENTAL FATFS f_open/f_read requested");
      uint32_t crc = 0, size = 0;
      if (fatfs.read_known_file(crc, size)) {
        ESP_LOGI("atilla_storage", "FATFS FILE READ: bytes=%" PRIu32 " CRC32=%08" PRIX32,
                 size, crc);
      } else {
        ESP_LOGW("atilla_storage", "FATFS FILE OPEN/READ FAILED");
      }
    } else {
      ESP_LOGI("atilla_storage", "FATFS file open/read SKIPPED (crash isolation)");
    }
    const bool unmount_ok = fatfs.unmount();
    if (unmount_ok)
      ESP_LOGI("atilla_storage", "FATFS unmounted");
    else
      ESP_LOGE("atilla_storage", "FATFS unmount FAILED; cache handoff blocked");
    if (cache_image::handoff_ready(cache_read_pilot_, unmount_ok,
                                   bool(verified_cache_png.data))) {
      // Transfer is explicitly one-way, and publication occurs only AFTER
      // successful f_unmount: no live FATFS handle escapes the worker.
      auto *handoff = new (std::nothrow) experimental::ReadOnlyFatFs::CachedPng(
          std::move(verified_cache_png));
      if (!handoff) {
        ESP_LOGE("atilla_storage", "CACHE HANDOFF OOM; dropping frame NO WRITES");
      } else {
        auto *old = cache_png_pending_.exchange(handoff, std::memory_order_acq_rel);
        delete old;
        ESP_LOGI("atilla_storage",
                 "CACHE PSRAM POST-UNMOUNT PASS published-to-main-loop NO WRITES");
      }
    }
  }
  void dump_config() override {
    ESP_LOGCONFIG("atilla_storage", "ATILLA external storage: FatFs opt-in=%s cache-write pilot=%s; no automatic downloads",
                  fatfs_mount_test_ ? "YES" : "NO", cache_write_pilot_ ? "ENABLED" : "DISABLED");
  }
 protected:
  spi_device::SPIDeviceComponent *card_{nullptr};
  spi_device::SPIDeviceComponent *idle_{nullptr};
  uint32_t start_ms_{0};
  bool attempted_{false};
  bool fatfs_mount_test_{false};
  bool fatfs_file_read_test_{false};
  bool fatfs_entry_probe_{false};
  bool fatfs_fil_heap_probe_{false};
  bool fatfs_open_only_probe_{false};
  bool fatfs_batch_read_probe_{false};
  bool asset_discovery_probe_{false};
  bool bounded_crc_probe_{false};
  bool combined_readonly_probe_{false};
  bool manifest_readonly_probe_{false};
  bool cache_write_pilot_{false};
  bool https_cache_pilot_{false};
  bool cache_read_pilot_{false};
  std::atomic<experimental::ReadOnlyFatFs::CachedPng *> cache_png_pending_{nullptr};
  bool fatfs_write_roundtrip_test_{false};
  bool storage_service_test_{false};
  bool storage_repeat_test_{false};
  bool settings_persistence_probe_{false};
};
} // namespace esphome::atilla_storage

