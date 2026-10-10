#pragma once
// EXPERIMENTAL: development branch only; opt-in read-only FatFs interface.
// Target: ESP-IDF FatFs custom disk I/O. All writes explicitly refused.
// ESPHome 2026.9.1 CI compiled; physical-device file open unverified.
// WARNING: no formatting or writes; do not enable unattended filesystem access.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>
#include <new>
#include "esphome/core/hal.h"  // millis() timing diagnostics
#include "esp_err.h"
#include "ff.h"
#include "diskio.h"
#include "diskio_impl.h"
#include "atilla_storage.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace esphome::atilla_storage::experimental {
class ReadOnlyFatFs {
 public:
  explicit ReadOnlyFatFs(ReadOnlySD &sd) : sd_(sd) {}
  bool mount() {
    if (active_ != nullptr || !sd_.initialized()) return false;
    BYTE chosen = FF_DRV_NOT_USED;
    if (ff_diskio_get_drive(&chosen) != ESP_OK) return false;
    if (chosen >= FF_VOLUMES) return false;
    drive_ = chosen;
    active_ = this;
    static const ff_diskio_impl_t io = {
      .init = &disk_init_,
      .status = &disk_status_,
      .read = &disk_read_,
      .write = &disk_write_,
      .ioctl = &disk_ioctl_,
    };
    ff_diskio_register(drive_, &io);
    // FF_VOLUMES is currently capped at ten by ESP-IDF FatFs.
    std::snprintf(drive_path_, sizeof(drive_path_), "%u:", unsigned(drive_));
    // No f_mkfs! A mount failure must leave the SD untouched.
    const uint32_t mount_started = millis();
    const FRESULT rc = f_mount(&fs_, drive_path_, 1);
    ESP_LOGI("atilla_storage", "FATFS f_mount result=%u elapsed=%" PRIu32 " ms",
             unsigned(rc), uint32_t(millis() - mount_started));
    if (rc != FR_OK) {
      ff_diskio_register(drive_, nullptr);
      active_ = nullptr;
      drive_ = FF_DRV_NOT_USED;
      return false;
    }
    mounted_ = true;
    return true;
  }
  bool probe_file_object_allocation() {
    // Isolates FIL construction and heap availability from f_open and f_read.
    ESP_LOGI("atilla_storage", "FATFS FIL probe entered (no file operations)");
    ESP_LOGI("atilla_storage", "FATFS FIL probe stack minimum-free=%u bytes",
             unsigned(uxTaskGetStackHighWaterMark(nullptr) * sizeof(StackType_t)));
    if (!mounted_) return false;
    auto file = std::unique_ptr<FIL>(new (std::nothrow) FIL{});
    if (!file) {
      ESP_LOGE("atilla_storage", "FATFS FIL probe: heap allocation FAILED");
      return false;
    }
    ESP_LOGI("atilla_storage", "FATFS FIL probe PASS: heap object %u bytes, no f_open/f_read",
             unsigned(sizeof(FIL)));
    ESP_LOGI("atilla_storage", "FATFS FIL probe post-allocation stack minimum-free=%u bytes",
             unsigned(uxTaskGetStackHighWaterMark(nullptr) * sizeof(StackType_t)));
    return true;
  }
  bool read_known_file(uint32_t &crc, uint32_t &size) {
    // Previous device firmware rebooted before the first log in this function.
    // Keep the potentially large FatFs FIL allocation off the caller's stack.
    ESP_LOGI("atilla_storage", "FATFS read function entered; no file opened yet");
    if (!mounted_) {
      ESP_LOGW("atilla_storage", "FATFS read refused: filesystem not mounted");
      return false;
    }
    auto file = std::unique_ptr<FIL>(new (std::nothrow) FIL{});
    if (!file) {
      ESP_LOGE("atilla_storage", "FATFS FIL heap allocation failed; no file opened");
      return false;
    }
    ESP_LOGI("atilla_storage", "FATFS FIL allocated outside task stack (%u bytes)",
             unsigned(sizeof(FIL)));
    char pathname[80];
    std::snprintf(pathname, sizeof(pathname), "%s/SYSTEM~1/WPSETT~1.DAT", drive_path_);
    ESP_LOGI("atilla_storage", "FATFS opening existing file (read only)");
    const uint32_t open_started = millis();
    const FRESULT open_rc = f_open(file.get(), pathname, FA_READ);
    ESP_LOGI("atilla_storage", "FATFS f_open result=%u elapsed=%" PRIu32 " ms",
             unsigned(open_rc), uint32_t(millis() - open_started));
    if (open_rc != FR_OK) return false;
    const FSIZE_t expected_size = 12;
    const FSIZE_t actual_size = f_size(file.get());
    ESP_LOGI("atilla_storage", "FATFS f_size=%" PRIu32 " expected=12", static_cast<uint32_t>(actual_size));
    if (actual_size != expected_size) {
      ESP_LOGW("atilla_storage", "FATFS unexpected existing file size; closing without reading");
      (void) f_close(file.get());
      return false;
    }
    uint8_t buf[12] = {};
    UINT got = 0;
    ESP_LOGI("atilla_storage", "FATFS f_read starting");
    const uint32_t read_started = millis();
    const FRESULT read_rc = f_read(file.get(), buf, 12, &got);
    ESP_LOGI("atilla_storage", "FATFS f_read result=%u bytes=%u elapsed=%" PRIu32 " ms",
             unsigned(read_rc), unsigned(got), uint32_t(millis() - read_started));
    const uint32_t close_started = millis();
    const FRESULT close_rc = f_close(file.get());
    ESP_LOGI("atilla_storage", "FATFS f_close result=%u elapsed=%" PRIu32 " ms",
             unsigned(close_rc), uint32_t(millis() - close_started));
    const bool ok = read_rc == FR_OK && got == 12 && close_rc == FR_OK;
    if (!ok) return false;
    uint32_t x = 0xffffffffU;
    for (unsigned i = 0; i < got; ++i) {
      x ^= buf[i];
      for (int b=0;b<8;b++) x = (x >> 1) ^ ((x & 1) ? 0xedb88320U : 0);
    }
    crc = x ^ 0xffffffffU;
    size = got;
    if (crc != 0x630105D1U) {
      ESP_LOGW("atilla_storage", "FATFS CRC mismatch: got=%08" PRIX32 " expected=630105D1", crc);
      return false;
    }
    ESP_LOGI("atilla_storage", "FATFS file bytes verified against known-good raw-sector CRC");
    return true;
  }
  void unmount() {
    if (!mounted_) {
      if (active_ == this) {
        ff_diskio_register(drive_, nullptr);
        active_ = nullptr;
        drive_ = FF_DRV_NOT_USED;
      }
      return;
    }
    const uint32_t unmount_started = millis();
    const FRESULT unmount_rc = f_mount(nullptr, drive_path_, 0);
    ESP_LOGI("atilla_storage", "FATFS f_unmount result=%u elapsed=%" PRIu32 " ms",
             unsigned(unmount_rc), uint32_t(millis() - unmount_started));
    ff_diskio_register(drive_, nullptr);
    mounted_ = false;
    active_ = nullptr;
    drive_ = FF_DRV_NOT_USED;
  }
  ~ReadOnlyFatFs() { unmount(); }
  ReadOnlyFatFs(const ReadOnlyFatFs &) = delete;
  ReadOnlyFatFs &operator=(const ReadOnlyFatFs &) = delete;
 private:
  static DSTATUS disk_init_(BYTE pdrv) { return disk_status_(pdrv); }
  static DSTATUS disk_status_(BYTE pdrv) {
    return active_ && active_->drive_ == pdrv && active_->sd_.initialized()
      ? STA_PROTECT : STA_NOINIT;
  }
  static DRESULT disk_read_(BYTE pdrv, BYTE *out, uint32_t sector, UINT count) {
    if ((disk_status_(pdrv) & STA_NOINIT) || !out || !count) return RES_NOTRDY;
    if (uint64_t(sector) + count > active_->sd_.physical_sector_limit())
      return RES_PARERR;
    for (UINT i = 0; i < count; i++)
      if (!active_->sd_.read_physical_sector(sector + i, out + 512U * i))
        return RES_ERROR;
    return RES_OK;
  }
  static DRESULT disk_write_(BYTE, const BYTE *, uint32_t, UINT) {
    return RES_WRPRT;
  }
  static DRESULT disk_ioctl_(BYTE pdrv, BYTE cmd, void *out) {
    if (disk_status_(pdrv) & STA_NOINIT) return RES_NOTRDY;
    if (cmd == CTRL_SYNC) return RES_OK;
    if (out == nullptr) return RES_PARERR;
    switch (cmd) {
      case GET_SECTOR_SIZE:
        *static_cast<WORD *>(out) = 512;
        return RES_OK;
      case GET_SECTOR_COUNT:
        // Conservative bounds. Real card CSD still needs parsing before writing.
        *static_cast<DWORD *>(out) =
          static_cast<DWORD>(active_->sd_.physical_sector_limit());
        return RES_OK;
      case GET_BLOCK_SIZE:
        *static_cast<DWORD *>(out) = 64;
        return RES_OK;
      default: return RES_PARERR;
    }
  }
  ReadOnlySD &sd_;
  FATFS fs_{};
  BYTE drive_{FF_DRV_NOT_USED};
  char drive_path_[8]{};
  bool mounted_{false};
  inline static ReadOnlyFatFs *active_{nullptr};
};
} // namespace esphome::atilla_storage::experimental
