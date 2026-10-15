#pragma once
// Explicit opt-in HTTPS fetch of one public immutable 70-byte PNG fixture.
// TLS certificate chain and exact response byte count+CRC must all validate.
// No redirects, arbitrary URLs, secrets, SD writes or LVGL access here.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <ctime>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_heap_caps.h"
#include "esp_timer.h"
#include "esphome/core/log.h"
#include "graphics_download_contract.h"

namespace esphome::atilla_storage::graphics_https {
struct FreeBuffer {
  void operator()(uint8_t *p) const { if (p) heap_caps_free(p); }
};
using Buffer = std::unique_ptr<uint8_t, FreeBuffer>;
struct Download {
  Buffer bytes{};
  uint32_t size{0};
};

inline bool download_public_test_asset(Download &out) {
  out = Download{};
  if (!graphics_fetch::approved_url(graphics_fetch::kUrl)) return false;
  // The panel can start at 1969/1970 before the HA time source synchronizes.
  // Wait off the main loop; fail closed (and leave SD untouched) after 45s.
  const int64_t clock_start_us = esp_timer_get_time();
  bool reported_clock_wait = false;
  while (!graphics_fetch::tls_clock_ready(static_cast<int64_t>(std::time(nullptr)))) {
    if (!reported_clock_wait) {
      ESP_LOGI("atilla_storage", "HTTPS CACHE: waiting for system clock before TLS");
      reported_clock_wait = true;
    }
    if (esp_timer_get_time() - clock_start_us > 45000000LL) {
      ESP_LOGW("atilla_storage", "HTTPS CACHE: no valid clock; skipping download and SD write");
      return false;
    }
    vTaskDelay(pdMS_TO_TICKS(250));
  }
  // Allocate in PSRAM, never reserve a 1 MiB network response on main-loop stack.
  Buffer bytes(static_cast<uint8_t *>(heap_caps_malloc(
      graphics_fetch::kExpectedBytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT)));
  if (!bytes) {
    ESP_LOGW("atilla_storage", "HTTPS CACHE: PSRAM allocation failed");
    return false;
  }
  esp_http_client_config_t config{};
  config.url = graphics_fetch::kUrl;
  config.method = HTTP_METHOD_GET;
  config.timeout_ms = 12000;
  config.crt_bundle_attach = esp_crt_bundle_attach;
  config.disable_auto_redirect = true;
  config.buffer_size = 512;
  auto *http = esp_http_client_init(&config);
  if (!http) return false;
  bool ok = esp_http_client_open(http, 0) == ESP_OK;
  if (ok) {
    const int64_t header_len = esp_http_client_fetch_headers(http);
    const int status = esp_http_client_get_status_code(http);
    const bool chunked = esp_http_client_is_chunked_response(http);
    graphics_fetch::BoundedReceiver receiver(bytes.get(),
        graphics_fetch::kExpectedBytes, graphics_fetch::kExpectedCrc32);
    ok = receiver.begin(status, header_len, chunked);
    const int64_t start_us = esp_timer_get_time();
    uint8_t packet[256];
    while (ok && receiver.received() < graphics_fetch::kExpectedBytes) {
      if (esp_timer_get_time() - start_us > 25000000LL) {
        ok = false;
        break;
      }
      const int max_read = static_cast<int>(
          (graphics_fetch::kExpectedBytes - receiver.received()) < sizeof(packet)
              ? graphics_fetch::kExpectedBytes - receiver.received() : sizeof(packet));
      const int n = esp_http_client_read(http, reinterpret_cast<char *>(packet), max_read);
      if (n <= 0 || !receiver.feed(packet, static_cast<size_t>(n))) {
        ok = false;
        break;
      }
    }
    ok = ok && receiver.complete();
  }
  // Ensure TLS buffers/sockets are released before the SD card is mounted.
  (void) esp_http_client_close(http);
  (void) esp_http_client_cleanup(http);
  if (!ok) {
    ESP_LOGW("atilla_storage",
             "HTTPS CACHE FAIL: trusted test PNG network/TLS/length/CRC rejected");
    return false;
  }
  out.bytes = std::move(bytes);
  out.size = graphics_fetch::kExpectedBytes;
  ESP_LOGI("atilla_storage",
           "HTTPS CACHE DOWNLOAD PASS bytes=%" PRIu32 " crc32=%08" PRIX32
           " tls=VERIFIED redirects=DISABLED",
           out.size, graphics_fetch::kExpectedCrc32);
  return true;
}
}  // namespace esphome::atilla_storage::graphics_https
