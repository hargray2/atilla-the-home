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
             unsigned(uxTaskGetStackHighWaterMark(nullptr)));
    if (!mounted_) return false;
    auto file = std::unique_ptr<FIL>(new (std::nothrow) FIL{});
    if (!file) {
      ESP_LOGE("atilla_storage", "FATFS FIL probe: heap allocation FAILED");
      return false;
    }
    ESP_LOGI("atilla_storage", "FATFS FIL probe PASS: heap object %u bytes, no f_open/f_read",
             unsigned(sizeof(FIL)));
    ESP_LOGI("atilla_storage", "FATFS FIL probe post-allocation stack minimum-free=%u bytes",
             unsigned(uxTaskGetStackHighWaterMark(nullptr)));
    return true;
  }
  bool probe_existing_file_open_only() {
    // Distinct stage after heap allocation success: DO NOT read file contents.
    // Still experimental: f_open can consume stack and previously correlated with reboot.
    ESP_LOGI("atilla_storage", "FATFS OPEN-ONLY probe entered, stack minimum-free=%u bytes",
             unsigned(uxTaskGetStackHighWaterMark(nullptr)));
    if (!mounted_) return false;
    auto file = std::unique_ptr<FIL>(new (std::nothrow) FIL{});
    if (!file) {
      ESP_LOGE("atilla_storage", "FATFS OPEN-ONLY heap allocation failed");
      return false;
    }
    char pathname[80];
    std::snprintf(pathname, sizeof(pathname), "%s/SYSTEM~1/WPSETT~1.DAT", drive_path_);
    ESP_LOGI("atilla_storage", "FATFS OPEN-ONLY before f_open; stack minimum-free=%u bytes",
             unsigned(uxTaskGetStackHighWaterMark(nullptr)));
    const uint32_t started = millis();
    const FRESULT rc = f_open(file.get(), pathname, FA_READ);
    ESP_LOGI("atilla_storage", "FATFS OPEN-ONLY f_open result=%u elapsed=%" PRIu32
             " ms, stack minimum-free=%u bytes",
             unsigned(rc), uint32_t(millis() - started),
             unsigned(uxTaskGetStackHighWaterMark(nullptr)));
    if (rc != FR_OK) return false;
    const FRESULT close_rc = f_close(file.get());
    ESP_LOGI("atilla_storage", "FATFS OPEN-ONLY f_close result=%u; no f_read attempted",
             unsigned(close_rc));
    return close_rc == FR_OK;
  }
  bool run_settings_persistence_probe() {
    // Non-destructive persistence probe: never overwrites or deletes preexisting data.
    if (!mounted_ || !sd_.verify_physical_capacity()) return false;
    char dir[32], name[48];
    std::snprintf(dir,sizeof(dir),"%s/ATILLA",drive_path_);
    std::snprintf(name,sizeof(name),"%s/ATILLA/SETT01.TST",drive_path_);
    auto info=std::unique_ptr<FILINFO>(new(std::nothrow) FILINFO{});
    auto file=std::unique_ptr<FIL>(new(std::nothrow) FIL{});
    if(!info || !file) return false;
    FRESULT rc=f_stat(dir,info.get());
    if(rc!=FR_OK || !(info->fattrib & AM_DIR)) {
      ESP_LOGE("atilla_storage","SETTINGS missing /ATILLA directory rc=%u",unsigned(rc));
      return false;
    }
    // A deterministic, fixed-width record. CRC detects torn/corrupt writes.
    uint8_t record[32]={};
    const char magic[8]={'A','T','I','L','L','A','S','1'};
    std::memcpy(record,magic,8);
    record[8]=1; // record schema
    record[9]=25; // SAMPLE ambient brightness; never applied to display
    record[10]=2; // SAMPLE dim-after minutes
    record[11]=5; // SAMPLE screen-off minutes
    for(unsigned i=12;i<28;++i) record[i]=uint8_t(i*7U+3U);
    uint32_t crc=0xffffffffU;
    for(unsigned i=0;i<28;++i) {
      crc^=record[i];
      for(unsigned bit=0;bit<8;++bit)
        crc=(crc>>1)^((crc&1U)?0xedb88320U:0U);
    }
    crc^=0xffffffffU;
    for(unsigned i=0;i<4;++i)record[28+i]=uint8_t(crc>>(i*8));
    rc=f_stat(name,info.get());
    if(rc==FR_OK) {
      ESP_LOGI("atilla_storage","SETTINGS existing record found, checking persisted data without writing");
      if(info->fsize!=32 || (info->fattrib & AM_DIR)) return false;
      const FRESULT op=f_open(file.get(),name,FA_READ);
      if(op!=FR_OK)return false;
      uint8_t saved[32]={};
      UINT got=0;
      const FRESULT rd=f_read(file.get(),saved,32,&got);
      const FRESULT cl=f_close(file.get());
      const bool match=rd==FR_OK && cl==FR_OK && got==32 && std::memcmp(saved,record,32)==0;
      ESP_LOGI("atilla_storage","SETTINGS PERSISTENCE %s (existing record across boot; read-only)",
               match?"PASS":"FAIL");
      return match;
    }
    if(rc!=FR_NO_FILE) {
      ESP_LOGE("atilla_storage","SETTINGS unexpected f_stat=%u; refusing write",unsigned(rc));
      return false;
    }
    if(!sd_.enable_test_writes())return false;
    writes_active_=true;
    ESP_LOGI("atilla_storage","SETTINGS creating new immutable sample record");
    rc=f_open(file.get(),name,FA_WRITE|FA_CREATE_NEW);
    if(rc!=FR_OK)return false;
    UINT written=0;
    const FRESULT wr=f_write(file.get(),record,32,&written);
    const FRESULT sync=wr==FR_OK && written==32?f_sync(file.get()):FR_DISK_ERR;
    const FRESULT cl=f_close(file.get());
    ESP_LOGI("atilla_storage","SETTINGS write=%u written=%u sync=%u close=%u",
             unsigned(wr),unsigned(written),unsigned(sync),unsigned(cl));
    if(wr!=FR_OK || written!=32 || sync!=FR_OK || cl!=FR_OK)return false;
    file.reset(new(std::nothrow) FIL{});
    if(!file)return false;
    rc=f_open(file.get(),name,FA_READ);
    if(rc!=FR_OK)return false;
    uint8_t saved[32]={};
    UINT got=0;
    const FRESULT rd=f_read(file.get(),saved,32,&got);
    const FRESULT closed=f_close(file.get());
    const bool match=rd==FR_OK && got==32 && closed==FR_OK && std::memcmp(saved,record,32)==0;
    ESP_LOGI("atilla_storage","SETTINGS PERSISTENCE %s (first write + immediate readback, retains SETT01.TST)",
             match?"PASS":"FAIL");
    return match;
  }
  bool run_repeated_write_test() {
    // Three distinct 512-byte files. Never overwrite or delete any pre-existing file.
    if (!mounted_ || !sd_.verify_physical_capacity()) return false;
    char folder[32];
    std::snprintf(folder,sizeof(folder),"%s/ATILLA",drive_path_);
    auto info=std::unique_ptr<FILINFO>(new (std::nothrow) FILINFO{});
    auto file=std::unique_ptr<FIL>(new (std::nothrow) FIL{});
    auto block=std::unique_ptr<uint8_t[]>(new (std::nothrow) uint8_t[512]);
    if (!info || !file || !block) return false;
    FRESULT rc=f_stat(folder,info.get());
    if (rc!=FR_OK || !(info->fattrib & AM_DIR)) {
      ESP_LOGE("atilla_storage","REPEAT abort: /ATILLA directory missing or invalid rc=%u",unsigned(rc));
      return false;
    }
    // Preflight all names BEFORE enabling writes; existing files are immutable.
    for(unsigned round=1;round<=3;++round) {
      char name[48];
      std::snprintf(name,sizeof(name),"%s/ATILLA/RPT%02u.TST",drive_path_,round);
      rc=f_stat(name,info.get());
      if(rc!=FR_NO_FILE) {
        ESP_LOGW("atilla_storage","REPEAT abort: test file occupied or stat failed round=%u rc=%u",round,unsigned(rc));
        return false;
      }
    }
    if(!sd_.enable_test_writes())return false;
    writes_active_=true;
    for(unsigned round=1;round<=3;++round) {
      char name[48];
      std::snprintf(name,sizeof(name),"%s/ATILLA/RPT%02u.TST",drive_path_,round);
      for(unsigned i=0;i<512;++i)
        block[i]=uint8_t((round*41U+i*17U+0x35U)&0xffU);
      ESP_LOGI("atilla_storage","REPEAT %u/3 create-new 512-byte test file",round);
      file.reset(new (std::nothrow) FIL{});
      if(!file)return false;
      rc=f_open(file.get(),name,FA_WRITE|FA_CREATE_NEW);
      if(rc!=FR_OK) {ESP_LOGE("atilla_storage","REPEAT create failed=%u",unsigned(rc));return false;}
      UINT count=0;
      const FRESULT wr=f_write(file.get(),block.get(),512,&count);
      const FRESULT sync=(wr==FR_OK&&count==512)?f_sync(file.get()):FR_DISK_ERR;
      const FRESULT close=f_close(file.get());
      ESP_LOGI("atilla_storage","REPEAT %u/3 write=%u bytes=%u sync=%u close=%u",
               round,unsigned(wr),unsigned(count),unsigned(sync),unsigned(close));
      if(wr!=FR_OK||count!=512||sync!=FR_OK||close!=FR_OK)return false;
      file.reset(new (std::nothrow) FIL{});
      if(!file)return false;
      rc=f_open(file.get(),name,FA_READ);
      ESP_LOGI("atilla_storage","REPEAT %u/3 reopen=%u",round,unsigned(rc));
      if(rc!=FR_OK)return false;
      UINT got=0;
      const FRESULT rd=f_read(file.get(),block.get(),512,&got);
      const FRESULT read_close=f_close(file.get());
      bool verified=(rd==FR_OK&&got==512&&read_close==FR_OK);
      if(verified)for(unsigned i=0;i<512;++i)
        if(block[i]!=uint8_t((round*41U+i*17U+0x35U)&0xffU)){verified=false;break;}
      ESP_LOGI("atilla_storage","REPEAT %u/3 verify=%s rd=%u got=%u",round,
               verified?"PASS":"FAIL",unsigned(rd),unsigned(got));
      if(!verified)return false; // Retain files for diagnosis after a failed round.
      rc=f_unlink(name);
      if(rc!=FR_OK)return false;
      rc=f_stat(name,info.get());
      if(rc!=FR_NO_FILE)return false;
      ESP_LOGI("atilla_storage","REPEAT %u/3 delete confirmed; stack min-free=%u bytes",
               round,unsigned(uxTaskGetStackHighWaterMark(nullptr)));
      vTaskDelay(pdMS_TO_TICKS(25));
    }
    ESP_LOGI("atilla_storage","REPEAT 3/3 PASS: independent 512-byte roundtrips completed");
    return true;
  }
  bool run_storage_service_test() {
    // Experimental 4 KiB multi-sector integrity check. No existing files overwritten.
    ESP_LOGI("atilla_storage", "SERVICE 1/8 validate SD physical capacity");
    if (!mounted_ || !sd_.verify_physical_capacity() || !sd_.enable_test_writes()) return false;
    writes_active_ = true;
    char folder[32], filename[48];
    std::snprintf(folder, sizeof(folder), "%s/ATILLA", drive_path_);
    std::snprintf(filename, sizeof(filename), "%s/ATILLA/VERIFY.BIN", drive_path_);
    ESP_LOGI("atilla_storage", "SERVICE 2/8 create or reuse ATILLA folder");
    FRESULT rc = f_mkdir(folder);
    if (rc == FR_EXIST) {
      auto info = std::unique_ptr<FILINFO>(new (std::nothrow) FILINFO{});
      if (!info || f_stat(folder, info.get()) != FR_OK || !(info->fattrib & AM_DIR)) {
        ESP_LOGE("atilla_storage", "SERVICE path exists but is not a directory");
        return false;
      }
    } else if (rc != FR_OK) {
      ESP_LOGE("atilla_storage", "SERVICE f_mkdir failure=%u", unsigned(rc));
      return false;
    }
    auto file = std::unique_ptr<FIL>(new (std::nothrow) FIL{});
    auto buffer = std::unique_ptr<uint8_t[]>(new (std::nothrow) uint8_t[512]);
    if (!file || !buffer) return false;
    // A previous watchdog may leave a complete VERIFY.BIN. Never overwrite
    // or remove that evidence; verify it in read-only mode on the next run.
    auto existing = std::unique_ptr<FILINFO>(new (std::nothrow) FILINFO{});
    if (!existing) return false;
    const FRESULT existing_rc = f_stat(filename, existing.get());
    ESP_LOGI("atilla_storage", "SERVICE prior-file stat=%u", unsigned(existing_rc));
    if (existing_rc == FR_OK) {
      ESP_LOGW("atilla_storage", "SERVICE existing VERIFY.BIN detected (%" PRIu32 " bytes); validating read-only and preserving it",
               uint32_t(existing->fsize));
      // Previously interrupted write: do not overwrite, remove or modify it.
      writes_active_ = false;
      sd_.disable_test_writes();
      if (existing->fsize != 4096 || (existing->fattrib & AM_DIR)) {
        ESP_LOGW("atilla_storage", "SERVICE existing file has unexpected size/type; preserved without changes");
        return false;
      }
      auto previous = std::unique_ptr<FIL>(new (std::nothrow) FIL{});
      auto buf = std::unique_ptr<uint8_t[]>(new (std::nothrow) uint8_t[512]);
      if (!previous || !buf) return false;
      ESP_LOGI("atilla_storage", "SERVICE existing file read-only f_open START");
      const FRESULT open_rc = f_open(previous.get(), filename, FA_READ);
      ESP_LOGI("atilla_storage", "SERVICE existing file f_open=%u", unsigned(open_rc));
      if (open_rc != FR_OK) return false;
      bool matches = true;
      for (unsigned block = 0; block < 8; ++block) {
        ESP_LOGI("atilla_storage", "SERVICE existing read block=%u START", block);
        UINT got=0;
        const FRESULT rd=f_read(previous.get(),buf.get(),512,&got);
        ESP_LOGI("atilla_storage", "SERVICE existing read block=%u rc=%u bytes=%u",
                 block,unsigned(rd),unsigned(got));
        if (rd != FR_OK || got != 512) { matches=false; break; }
        for (unsigned i=0;i<512;++i) {
          if(buf[i] != uint8_t((block*73U+i*19U+0x5aU)&0xffU)) {
            ESP_LOGE("atilla_storage", "SERVICE existing mismatch block=%u offset=%u", block,i);
            matches=false;
            break;
          }
        }
        if (!matches) break;
      }
      const FRESULT close_rc=f_close(previous.get());
      ESP_LOGI("atilla_storage", "SERVICE existing-file recovery %s close=%u; file preserved; task stack-min=%u bytes",
               matches && close_rc==FR_OK ? "PASS":"FAIL",
               unsigned(close_rc),unsigned(uxTaskGetStackHighWaterMark(nullptr)));
      return matches && close_rc==FR_OK;
    }
    if (existing_rc != FR_NO_FILE) {
      ESP_LOGE("atilla_storage", "SERVICE unexpected stat error=%u, refusing write", unsigned(existing_rc));
      return false;
    }
    ESP_LOGI("atilla_storage", "SERVICE 3/8 create-new VERIFY.BIN");
    rc = f_open(file.get(), filename, FA_WRITE | FA_CREATE_NEW);
    if (rc != FR_OK) {
      ESP_LOGW("atilla_storage", "SERVICE create-new=%u (existing test file never overwritten)",unsigned(rc));
      return false;
    }
    uint32_t expected_crc=0xffffffffU;
    bool write_ok=true;
    for(unsigned block=0; block<8; ++block) {
      for(unsigned i=0;i<512;++i) {
        uint8_t val=uint8_t((block*73U+i*19U+0x5aU) & 0xffU);
        buffer[i]=val;
        expected_crc ^= val;
        for(unsigned bit=0;bit<8;++bit)
          expected_crc=(expected_crc>>1)^((expected_crc&1U)?0xedb88320U:0U);
      }
      UINT written=0;
      const FRESULT wr=f_write(file.get(),buffer.get(),512,&written);
      if(wr!=FR_OK || written!=512) {
        ESP_LOGE("atilla_storage","SERVICE write block=%u rc=%u written=%u",block,unsigned(wr),unsigned(written));
        write_ok=false;break;
      }
    }
    expected_crc ^= 0xffffffffU;
    ESP_LOGI("atilla_storage", "SERVICE 4/8 write 4096 bytes result=%s",write_ok?"PASS":"FAIL");
    const FRESULT sync_rc=write_ok?f_sync(file.get()):FR_DISK_ERR;
    const FRESULT close_rc=f_close(file.get());
    ESP_LOGI("atilla_storage", "SERVICE sync=%u close=%u stack_min_free=%u",
             unsigned(sync_rc), unsigned(close_rc),
             unsigned(uxTaskGetStackHighWaterMark(nullptr)));
    if(!write_ok||sync_rc!=FR_OK||close_rc!=FR_OK) return false;
    file.reset(new (std::nothrow) FIL{});
    if (!file) return false;
    ESP_LOGI("atilla_storage", "SERVICE 5/8 reopening existing file (before f_open)");
    const uint32_t reopen_started=millis();
    rc = f_open(file.get(),filename,FA_READ);
    ESP_LOGI("atilla_storage", "SERVICE f_open(read)=%u elapsed=%" PRIu32 "ms stack_min_free=%u",
             unsigned(rc),uint32_t(millis()-reopen_started),unsigned(uxTaskGetStackHighWaterMark(nullptr)));
    if(rc!=FR_OK) return false;
    const bool right_size = f_size(file.get())==4096;
    uint32_t actual_crc=0xffffffffU;
    bool read_ok=right_size;
    for(unsigned block=0;block<8&&read_ok;++block) {
      UINT got=0;
      ESP_LOGI("atilla_storage", "SERVICE read block=%u start",block);
      const uint32_t read_started=millis();
      const FRESULT rd=f_read(file.get(),buffer.get(),512,&got);
      ESP_LOGI("atilla_storage", "SERVICE read block=%u rc=%u got=%u elapsed=%" PRIu32 "ms",
               block,unsigned(rd),unsigned(got),uint32_t(millis()-read_started));
      if(rd!=FR_OK||got!=512){read_ok=false;break;}
      for(unsigned i=0;i<512;++i) {
        actual_crc ^= buffer[i];
        for(unsigned bit=0;bit<8;++bit)
          actual_crc=(actual_crc>>1)^((actual_crc&1U)?0xedb88320U:0U);
      }
    }
    actual_crc ^= 0xffffffffU;
    const FRESULT read_close_rc=f_close(file.get());
    const bool verified=read_ok&&read_close_rc==FR_OK&&actual_crc==expected_crc;
    ESP_LOGI("atilla_storage", "SERVICE 6/8 CRC expected=%08" PRIX32 " actual=%08" PRIX32 " %s",
             expected_crc,actual_crc,verified?"PASS":"FAIL");
    if(!verified) return false; // Retain failed test file for inspection.
    ESP_LOGI("atilla_storage", "SERVICE 7/8 remove only VERIFY.BIN");
    const FRESULT del_rc=f_unlink(filename);
    if(del_rc!=FR_OK)return false;
    auto info=std::unique_ptr<FILINFO>(new (std::nothrow) FILINFO{});
    if(!info)return false;
    const FRESULT stat_rc=f_stat(filename,info.get());
    const bool ok=stat_rc==FR_NO_FILE;
    ESP_LOGI("atilla_storage", "SERVICE 8/8 %s: 4096 bytes, folder /ATILLA preserved, min-free-stack=%u",
             ok?"PASS":"FAIL",unsigned(uxTaskGetStackHighWaterMark(nullptr)));
    return ok;
  }
  bool run_batched_write_roundtrip() {
    // May alter FAT allocation/directory sectors. Test with disposable/backup card.
    // A fixed, unique file name prevents overwriting existing user data.
    ESP_LOGI("atilla_storage", "WRITE BATCH 1/7 verify CSD capacity");
    if (!mounted_ || !sd_.verify_physical_capacity() || !sd_.enable_test_writes()) {
      ESP_LOGE("atilla_storage", "WRITE BATCH abort: capacity verification failed");
      return false;
    }
    writes_active_ = true;
    // This test file is unique to ATILLA. Never truncate or overwrite an existing file.
    const char *data = "ATILLA SD READ WRITE TEST v1";
    const UINT data_len = static_cast<UINT>(std::strlen(data));
    char pathname[40];
    std::snprintf(pathname,sizeof(pathname),"%s/ATILLA.TST",drive_path_);
    auto file = std::unique_ptr<FIL>(new (std::nothrow) FIL{});
    if (!file) { ESP_LOGE("atilla_storage","WRITE BATCH heap FIL failed"); return false; }
    ESP_LOGI("atilla_storage", "WRITE BATCH 2/7 create-new %s (no overwrite)", pathname);
    FRESULT rc = f_open(file.get(),pathname, FA_WRITE | FA_CREATE_NEW);
    ESP_LOGI("atilla_storage", "WRITE BATCH f_open(create-new)=%u",unsigned(rc));
    if (rc != FR_OK) return false;
    UINT written=0;
    ESP_LOGI("atilla_storage", "WRITE BATCH 3/7 f_write %u bytes",unsigned(data_len));
    const FRESULT wr=f_write(file.get(),data,data_len,&written);
    const FRESULT sync=(wr==FR_OK && written==data_len) ? f_sync(file.get()) : FR_DISK_ERR;
    const FRESULT closed=f_close(file.get());
    ESP_LOGI("atilla_storage", "WRITE BATCH write=%u bytes=%u sync=%u close=%u",
             unsigned(wr),unsigned(written),unsigned(sync),unsigned(closed));
    if (wr!=FR_OK || written!=data_len || sync!=FR_OK || closed!=FR_OK) return false;
    // Re-open to independently validate the bytes after a close.
    file.reset(new (std::nothrow) FIL{});
    if (!file) return false;
    ESP_LOGI("atilla_storage", "WRITE BATCH 4/7 reopen FA_READ");
    rc=f_open(file.get(),pathname,FA_READ);
    if (rc!=FR_OK) { ESP_LOGE("atilla_storage","WRITE BATCH reopen=%u",unsigned(rc)); return false; }
    char received[40]={};
    UINT got=0;
    const FRESULT rd=f_read(file.get(),received,data_len,&got);
    const FRESULT recl=f_close(file.get());
    const bool matches=(rd==FR_OK && recl==FR_OK && got==data_len &&
                        std::memcmp(received,data,data_len)==0);
    ESP_LOGI("atilla_storage", "WRITE BATCH 5/7 verified=%s read=%u got=%u close=%u",
             matches?"YES":"NO",unsigned(rd),unsigned(got),unsigned(recl));
    if (!matches) return false;
    ESP_LOGI("atilla_storage", "WRITE BATCH 6/7 delete dedicated test file");
    const FRESULT unlink_rc=f_unlink(pathname);
    ESP_LOGI("atilla_storage", "WRITE BATCH delete=%u",unsigned(unlink_rc));
    if(unlink_rc != FR_OK) return false;
    auto info=std::unique_ptr<FILINFO>(new (std::nothrow) FILINFO{});
    if(!info) return false;
    const FRESULT stat_rc=f_stat(pathname,info.get());
    ESP_LOGI("atilla_storage", "WRITE BATCH 7/7 verify removal stat=%u expected=%u",
             unsigned(stat_rc),unsigned(FR_NO_FILE));
    const bool ok=(stat_rc==FR_NO_FILE);
    ESP_LOGI("atilla_storage", "WRITE BATCH %s minimum-free stack=%u bytes",
             ok?"PASS":"FAIL",unsigned(uxTaskGetStackHighWaterMark(nullptr)));
    return ok;
  }
  // Opt-in discovery only. No file data reads, no writes, no SD formatting.
  // Must execute from the existing dedicated SD worker with FatFs mounted.
  // Read a known, existing 4 KiB diagnostics file without ever opening it for write.
  // Two independent bounded passes must yield the same CRC32; this checks repeatability,
  // NOT authenticity against an independently supplied checksum.
  bool run_bounded_file_crc_probe() {
    if (!mounted_) return false;
    constexpr uint32_t kExpectedBytes = 4096;
    constexpr UINT kChunk = 256;
    constexpr uint32_t kMaxElapsedMs = 10000;
    char name[40];
    std::snprintf(name, sizeof(name), "%s/ATILLA/VERIFY.BIN", drive_path_);
    auto file = std::unique_ptr<FIL>(new (std::nothrow) FIL{});
    if (!file) return false;
    uint32_t crc_pass[2] = {0, 0};
    for (unsigned pass = 0; pass != 2; ++pass) {
      const FRESULT opened = f_open(file.get(), name, FA_READ);
      if (opened != FR_OK) {
        ESP_LOGW("atilla_storage", "FILE CRC FAIL pass=%u open=%u", pass + 1, unsigned(opened));
        return false;
      }
      bool ok = (f_size(file.get()) == kExpectedBytes);
      uint32_t crc = 0xffffffffU;
      uint32_t total = 0;
      const uint32_t started = millis();
      uint8_t buffer[kChunk];
      while (ok && total < kExpectedBytes) {
        if (uint32_t(millis() - started) > kMaxElapsedMs) { ok = false; break; }
        UINT got = 0;
        const UINT requested = UINT((kExpectedBytes - total) < kChunk ?
                                    (kExpectedBytes - total) : kChunk);
        const FRESULT rd = f_read(file.get(), buffer, requested, &got);
        if (rd != FR_OK || got != requested) { ok = false; break; }
        for (UINT i = 0; i < got; ++i) {
          crc ^= buffer[i];
          for (unsigned bit = 0; bit < 8; ++bit)
            crc = (crc >> 1) ^ ((crc & 1U) ? 0xedb88320U : 0U);
        }
        total += got;
      }
      const FRESULT closed = f_close(file.get());
      if (!ok || closed != FR_OK || total != kExpectedBytes) {
        ESP_LOGW("atilla_storage", "FILE CRC FAIL pass=%u bytes=%" PRIu32 " close=%u",
                 pass + 1, total, unsigned(closed));
        return false;
      }
      crc_pass[pass] = crc ^ 0xffffffffU;
      ESP_LOGI("atilla_storage", "FILE CRC pass=%u bytes=%" PRIu32 " crc32=%08" PRIX32
               " chunks=16 stack_free=%u", pass + 1, total, crc_pass[pass],
               unsigned(uxTaskGetStackHighWaterMark(nullptr)));
    }
    const bool repeatable = crc_pass[0] == crc_pass[1];
    ESP_LOGI("atilla_storage", "FILE CRC READ-ONLY %s bytes=4096 crc32=%08" PRIX32
             " repeatable=%s NO WRITES", repeatable ? "PASS" : "FAIL",
             crc_pass[0], repeatable ? "YES" : "NO");
    return repeatable;
  }

  bool run_asset_discovery_read_only() {
    if (!mounted_) return false;
    auto dir = std::unique_ptr<FF_DIR>(new (std::nothrow) FF_DIR{});
    auto info = std::unique_ptr<FILINFO>(new (std::nothrow) FILINFO{});
    if (!dir || !info) {
      ESP_LOGE("atilla_storage", "ASSET DISCOVERY FAIL: directory allocation");
      return false;
    }
    char path[32];
    std::snprintf(path, sizeof(path), "%s/ATILLA", drive_path_);
    FRESULT rc = f_opendir(dir.get(), path);
    if (rc != FR_OK) {
      ESP_LOGW("atilla_storage", "ASSET DISCOVERY FAIL: /ATILLA unavailable rc=%u (no changes)",
               unsigned(rc));
      return false;
    }
    constexpr unsigned kMaxEntries = 32;
    unsigned entries = 0;
    unsigned files = 0;
    bool complete = false;
    bool ok = true;
    for (; entries < kMaxEntries; ++entries) {
      std::memset(info.get(), 0, sizeof(FILINFO));
      rc = f_readdir(dir.get(), info.get());
      if (rc != FR_OK) { ok = false; break; }
      if (info->fname[0] == 0) { complete = true; break; }
      // Never descend into directories or interpret names as paths.
      const bool directory = (info->fattrib & AM_DIR) != 0;
      if (!directory) ++files;
      ESP_LOGI("atilla_storage", "ASSET DISCOVERY entry=%u type=%s size=%" PRIu32 " name=%.64s",
               entries + 1, directory ? "DIR" : "FILE",
               static_cast<uint32_t>(info->fsize), info->fname);
    }
    // A full directory might have more entries. Report truncation, not completeness.
    const FRESULT close_rc = f_closedir(dir.get());
    ok = ok && (close_rc == FR_OK);
    ESP_LOGI("atilla_storage",
             "ASSET DISCOVERY %s entries=%u files=%u complete=%s readdir_rc=%u close_rc=%u stack_free=%u NO WRITES",
             ok ? "PASS" : "FAIL", entries, files, complete ? "YES" : "NO",
             unsigned(rc), unsigned(close_rc), unsigned(uxTaskGetStackHighWaterMark(nullptr)));
    return ok;
  }

  bool run_batched_read_only_diagnostics() {
    ESP_LOGI("atilla_storage", "BATCH 1/5 mounted=%s stack_min_free=%u bytes",
             mounted_ ? "YES" : "NO", unsigned(uxTaskGetStackHighWaterMark(nullptr)));
    if (!mounted_) return false;
    auto file = std::unique_ptr<FIL>(new (std::nothrow) FIL{});
    if (!file) { ESP_LOGE("atilla_storage", "BATCH FAIL heap FIL allocation"); return false; }
    ESP_LOGI("atilla_storage", "BATCH 2/5 FIL heap allocated=%u bytes; stack_min_free=%u",
             unsigned(sizeof(FIL)), unsigned(uxTaskGetStackHighWaterMark(nullptr)));
    char filename[80];
    std::snprintf(filename, sizeof(filename), "%s/SYSTEM~1/WPSETT~1.DAT", drive_path_);
    ESP_LOGI("atilla_storage", "BATCH 3/5 f_open FA_READ beginning");
    FRESULT rc = f_open(file.get(), filename, FA_READ);
    ESP_LOGI("atilla_storage", "BATCH 3/5 f_open result=%u stack_min_free=%u",
             unsigned(rc), unsigned(uxTaskGetStackHighWaterMark(nullptr)));
    if (rc != FR_OK) return false;
    const auto expected_size = f_size(file.get());
    ESP_LOGI("atilla_storage", "BATCH 4/5 f_size=%" PRIu32, uint32_t(expected_size));
    uint8_t bytes[12] = {};
    UINT read_bytes = 0;
    FRESULT read_rc = FR_INVALID_OBJECT;
    if (expected_size == 12) {
      ESP_LOGI("atilla_storage", "BATCH 4/5 f_read start (12 bytes)");
      read_rc = f_read(file.get(), bytes, 12, &read_bytes);
    }
    const FRESULT close_rc = f_close(file.get());
    ESP_LOGI("atilla_storage", "BATCH 4/5 read_rc=%u got=%u close_rc=%u stack_min_free=%u",
             unsigned(read_rc), unsigned(read_bytes), unsigned(close_rc),
             unsigned(uxTaskGetStackHighWaterMark(nullptr)));
    if (read_rc != FR_OK || read_bytes != 12 || close_rc != FR_OK) return false;
    uint32_t crc = 0xffffffffU;
    for (unsigned i = 0; i < 12; ++i) {
      crc ^= bytes[i];
      for (unsigned bit = 0; bit < 8; ++bit)
        crc = (crc >> 1) ^ ((crc & 1U) ? 0xedb88320U : 0U);
    }
    crc ^= 0xffffffffU;
    ESP_LOGI("atilla_storage", "BATCH 4/5 CRC32=%08" PRIX32 " expected=630105D1", crc);
    if (crc != 0x630105D1U) return false;
    auto dir = std::unique_ptr<FF_DIR>(new (std::nothrow) FF_DIR{});
    auto info = std::unique_ptr<FILINFO>(new (std::nothrow) FILINFO{});
    if (!dir || !info) { ESP_LOGE("atilla_storage", "BATCH FAIL heap directory allocation"); return false; }
    char root[12];
    std::snprintf(root, sizeof(root), "%s/", drive_path_);
    ESP_LOGI("atilla_storage", "BATCH 5/5 f_opendir root start");
    rc = f_opendir(dir.get(), root);
    ESP_LOGI("atilla_storage", "BATCH 5/5 f_opendir result=%u stack_min_free=%u",
             unsigned(rc), unsigned(uxTaskGetStackHighWaterMark(nullptr)));
    if (rc != FR_OK) return false;
    bool found = false;
    unsigned entries = 0;
    for (; entries < 16; ++entries) {
      std::memset(info.get(), 0, sizeof(FILINFO));
      rc = f_readdir(dir.get(), info.get());
      if (rc != FR_OK || info->fname[0] == 0) break;
      if (std::strcmp(info->fname, "SYSTEM~1") == 0) found = true;
      ESP_LOGI("atilla_storage", "BATCH directory entry %u: %s", entries + 1, info->fname);
    }
    const FRESULT dir_close_rc = f_closedir(dir.get());
    ESP_LOGI("atilla_storage", "BATCH 5/5 directory entries=%u found_system=%s readdir=%u close=%u",
             entries, found ? "YES" : "NO", unsigned(rc), unsigned(dir_close_rc));
    // Directory listing may use long filenames; success requires traversal, not a specific alias.
    const bool ok = rc == FR_OK && dir_close_rc == FR_OK;
    ESP_LOGI("atilla_storage", "BATCH READ-ONLY %s; no writes attempted; stack_min_free=%u",
             ok ? "PASS" : "FAIL", unsigned(uxTaskGetStackHighWaterMark(nullptr)));
    return ok;
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
    writes_active_ = false;
    sd_.disable_test_writes();
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
      ? (active_->writes_active_ ? 0 : STA_PROTECT) : STA_NOINIT;
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
  static DRESULT disk_write_(BYTE pdrv, const BYTE *in, uint32_t sector, UINT count) {
    if (!active_ || !active_->writes_active_) return RES_WRPRT;
    if ((disk_status_(pdrv) & STA_NOINIT) || !in || count==0 ||
        uint64_t(sector) < active_->sd_.partition_start() ||
        uint64_t(sector)+count > active_->sd_.physical_sector_limit()) return RES_PARERR;
    for (UINT i=0;i<count;i++)
      if (!active_->sd_.write_physical_sector(sector+i,in+512U*i)) return RES_ERROR;
    return RES_OK;
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
  bool writes_active_{false};
  inline static ReadOnlyFatFs *active_{nullptr};
};
} // namespace esphome::atilla_storage::experimental
