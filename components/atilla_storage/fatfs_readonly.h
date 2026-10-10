#pragma once
// EXPERIMENTAL DESIGN CANDIDATE — not included by the working component.
// Target: ESP-IDF FatFs custom disk I/O. All writes explicitly refused.
// This code has not been compiled against ATILLA's ESPHome/IDF toolchain.
// Build prerequisite: fatfs component headers and linkage must be available.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "esp_err.h"
#include "ff.h"
#include "diskio.h"
#include "diskio_impl.h"
#include "atilla_storage.h"

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
    const FRESULT rc = f_mount(&fs_, drive_path_, 1);
    if (rc != FR_OK) {
      ff_diskio_register(drive_, nullptr);
      active_ = nullptr;
      drive_ = FF_DRV_NOT_USED;
      return false;
    }
    mounted_ = true;
    return true;
  }
  bool read_known_file(uint32_t &crc, uint32_t &size) {
    if (!mounted_) return false;
    char pathname[80];
    std::snprintf(pathname, sizeof(pathname), "%s/SYSTEM~1/WPSETT~1.DAT", drive_path_);
    FIL file{};
    if (f_open(&file, pathname, FA_READ) != FR_OK) return false;
    uint8_t buf[64] = {};
    UINT got = 0;
    const bool ok = f_read(&file, buf, sizeof(buf), &got) == FR_OK && got == 12;
    (void) f_close(&file);
    if (!ok) return false;
    uint32_t x = 0xffffffffU;
    for (unsigned i = 0; i < got; ++i) {
      x ^= buf[i];
      for (int b=0;b<8;b++) x = (x >> 1) ^ ((x & 1) ? 0xedb88320U : 0);
    }
    crc = x ^ 0xffffffffU;
    size = got;
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
    (void) f_mount(nullptr, drive_path_, 0);
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
