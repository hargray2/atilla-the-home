#pragma once
// ATILLA THE HOME 4.6 — experimental READ-ONLY SD SPI transport.
// NOT FATFS. NOT YET COMPILE-TESTED ON ESPHome 2026.9.1.
// Existing ESPHome SPIDevice instances handle CH422G EXIO4 chip select.
#include <cstdint>
#include <cinttypes>
#include <cstring>
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
    initialized_=false;
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
  uint32_t partition_start() const {return partition_start_;}
  uint32_t partition_sectors() const {return partition_sectors_;}
  bool initialized() const {return initialized_;}
  // No write command or filesystem modification is exposed.
 private:
  Bus card_,idle_;
  bool initialized_{false};
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

// ESPHome codegen integration: one delayed, read-only probe per boot.
// In particular this does NOT mount FATFS or expose a write method.
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
  void setup() override {
    start_ms_ = millis();
    ESP_LOGI("atilla_storage", "ATILLA SD read-only probe scheduled for +30 seconds");
  }
  void loop() override {
    if (attempted_ || (uint32_t) (millis() - start_ms_) < 30000UL) return;
    attempted_ = true; // never repeat after a failure
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
  }
  void dump_config() override {
    ESP_LOGCONFIG("atilla_storage", "ATILLA external storage component: delayed READ-ONLY probe; no FATFS, no writes");
  }
 protected:
  spi_device::SPIDeviceComponent *card_{nullptr};
  spi_device::SPIDeviceComponent *idle_{nullptr};
  uint32_t start_ms_{0};
  bool attempted_{false};
};
} // namespace esphome::atilla_storage
