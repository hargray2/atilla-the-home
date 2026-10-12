#pragma once
// Read-only cached graphics acceptance contract. No filesystem operations or
// LVGL calls in this file. All byte buffers are caller-owned.
#include <cstddef>
#include <cstdint>
#include "graphics_download_contract.h"

namespace esphome::atilla_storage::cache_image {
constexpr uint32_t kMaxCompressedBytes = 262144U;  // 256 KiB per UI image
constexpr uint32_t kMaxPixels = 800U * 480U;
struct Metadata {
  uint16_t width{0};
  uint16_t height{0};
  uint32_t bytes{0};
  uint32_t crc32{0};
};
inline uint32_t crc32(const uint8_t *p, size_t size) {
  if (!p || size == 0 || size > kMaxCompressedBytes) return 0;
  uint32_t crc = 0xffffffffU;
  for (size_t i = 0; i < size; ++i) {
    crc ^= p[i];
    for (unsigned b = 0; b < 8; ++b)
      crc = (crc >> 1) ^ ((crc & 1U) ? 0xedb88320U : 0U);
  }
  return crc ^ 0xffffffffU;
}
inline uint32_t read_be32(const uint8_t *p) {
  return (uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) |
         (uint32_t(p[2]) << 8) | uint32_t(p[3]);
}
// This checks a bounded PNG envelope and expected exact CRC, not full PNG
// decoding. A later decoder must reject malformed chunks or decompression bombs.
inline bool inspect_verified_png(const uint8_t *data, uint32_t size,
                                uint32_t expected_crc, Metadata &out) {
  out = Metadata{};
  if (!data || size < 45 || size > kMaxCompressedBytes ||
      !graphics_fetch::png_header(data, size) ||
      !graphics_fetch::png_trailer(data, size)) return false;
  const uint32_t width = read_be32(data + 16), height = read_be32(data + 20);
  if (width == 0 || height == 0 || width > 800 || height > 480 ||
      uint64_t(width) * height > kMaxPixels) return false;
  if (crc32(data, size) != expected_crc) return false;
  out.width = static_cast<uint16_t>(width);
  out.height = static_cast<uint16_t>(height);
  out.bytes = size;
  out.crc32 = expected_crc;
  return true;
}
// Cache buffers can cross from the SD worker to the main loop only when the
// read pilot was requested, FatFs unmounted successfully, and data is owned.
inline bool handoff_ready(bool read_pilot_enabled, bool unmount_ok,
                          bool buffer_owned) {
  return read_pilot_enabled && unmount_ok && buffer_owned;
}
inline bool valid_request(unsigned slot, uint32_t len) {
  return slot >= 1 && slot <= 9999 && len >= 45 && len <= kMaxCompressedBytes;
}
}  // namespace esphome::atilla_storage::cache_image
