#pragma once
// ATILLA radar intensity filter prototype (RainViewer scheme 2: Universal Blue).
// Remove decoded returns below 35 dBZ from downloaded radar images only.
// Published color table: https://www.rainviewer.com/api/color-schemes.html
// NOT yet tested on ATILLA or wired into the private full YAML.
#include <cstddef>
#include <cstdint>
#include "esphome/components/online_image/online_image.h"

namespace esphome::atilla_radar_filter {
inline constexpr uint8_t kModerateDbz = 35;

// Palette RGB entries correspond to RainViewer Universal Blue dBZ -10..34.
// This is an exact palette lookup, not a hue classifier: dark map pixels
// must NEVER be processed through this function.
inline constexpr uint32_t kWeakRainRgb[] = {0x636159u, 0x66635au, 0x69665cu, 0x6c685du, 0x6f6b5fu, 0x726e61u, 0x757062u, 0x787364u, 0x7c7565u, 0x7f7867u, 0x005a00u, 0x006100u, 0x006800u, 0x006f00u, 0x007600u, 0x007e00u, 0x017e2bu, 0x037e57u, 0x047e83u, 0x067eafu, 0x087fdbu, 0x0c73ddu, 0x1068e0u, 0x145de2u, 0x1852e5u, 0x1c47e8u, 0x2c3be1u, 0x3c2fdau, 0x4d24d3u, 0x5d18ccu, 0x6e0dc6u, 0x800db9u, 0x920dacu, 0xa40e9fu, 0xb60e92u, 0xc80f86u, 0xc62086u, 0xc43186u, 0xc34286u, 0xc15386u, 0xc06487u, 0xc36b77u, 0xc77268u, 0xca7959u, 0xce804au};
static_assert(sizeof(kWeakRainRgb) / sizeof(kWeakRainRgb[0]) == 45);

inline bool is_weak_rgb(uint32_t rgb) {
  for (uint32_t entry : kWeakRainRgb) if (rgb == entry) return true;
  return false;
}

inline uint16_t to_rgb565(uint32_t rgb) {
  const uint8_t r = (rgb >> 16) & 0xFF, g = (rgb >> 8) & 0xFF, b = rgb & 0xFF;
  return uint16_t((r & 0xF8) << 8) | uint16_t((g & 0xFC) << 3) | uint16_t(b >> 3);
}

inline bool is_weak_rgb565(uint16_t rgb) {
  for (uint32_t entry : kWeakRainRgb) if (rgb == to_rgb565(entry)) return true;
  return false;
}

// Call only after a successful, freshly decoded online_image PNG tile.
// Original buffer is owned by RuntimeImage and is mutable in that lifecycle.
// RGB: BGRA 4 bytes/pixel; RGB565: little/big-endian pixel plane + alpha plane.
// Reject opaque formats: transparency is required to reveal the basemap.
// No allocations, no HTTPS requests, no additional image buffers.
inline bool apply_moderate_heavy(online_image::OnlineImage &image,
                                size_t *hidden_out = nullptr,
                                bool rgb565_big_endian = false) {
  if (hidden_out) *hidden_out = 0;
  if (!image.is_loaded() || image.is_decoding() || !image.has_transparency() ||
      image.get_width() <= 0 || image.get_height() <= 0) return false;
  const size_t count = size_t(image.get_width()) * size_t(image.get_height());
  if (count > 256u * 256u) return false;  // Guard expected radar tile dimensions.
  const auto type = image.get_type();
  if (type != image::IMAGE_TYPE_RGB && type != image::IMAGE_TYPE_RGB565) return false;
  // The intended buffer layout requires transparency: alpha_channel, not chroma key.
  // Guarding via has_transparency() alone is insufficient to distinguish those.
  auto *bytes = const_cast<uint8_t *>(image.get_data_start());
  if (!bytes) return false;
  size_t hidden = 0;
  if (type == image::IMAGE_TYPE_RGB) {
    if (image.get_bpp() != 32) return false;
    for (size_t i=0;i<count;i++) {
      uint8_t *p = bytes + i * 4u; // B,G,R,A
      const uint32_t rgb = (uint32_t(p[2]) << 16) | (uint32_t(p[1]) << 8) | p[0];
      if (p[3] && is_weak_rgb(rgb)) { p[3] = 0; hidden++; }
    }
  } else {
    if (image.get_bpp() != 24) return false;
    auto *alpha = bytes + count * 2u;
    for (size_t i=0;i<count;i++) {
      const uint16_t rgb = rgb565_big_endian
        ? uint16_t(uint16_t(bytes[2u*i]) << 8 | bytes[2u*i+1])
        : uint16_t(uint16_t(bytes[2u*i+1]) << 8 | bytes[2u*i]);
      if (alpha[i] && is_weak_rgb565(rgb)) { alpha[i] = 0; hidden++; }
    }
  }
  if (hidden_out) *hidden_out = hidden;
  return true;
}
} // namespace esphome::atilla_radar_filter
