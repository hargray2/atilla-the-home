#pragma once
// Immutable fixture catalog, not a general runtime tile cache.
// Centralizes the three hardware-proven SD slots and closes the trust boundary
// between a caller's requested asset and an approved network destination.
// This header deliberately owns no SD bus, FatFs handle, decoder or LVGL object.
#include <cstdint>
#include "cache_image_contract.h"
#include "color_asset_contract.h"
#include "graphics_download_contract.h"

namespace esphome::atilla_storage::cache_catalog {
enum class Id : uint8_t { BlueLocal = 1, BlueHttps = 2, ColorHttps = 3 };
struct Spec {
  Id id;
  unsigned slot;
  uint32_t bytes;
  uint32_t crc32;
  uint16_t width;
  uint16_t height;
  const char *url;  // nullptr => read-only; never download
};
inline const Spec *find(Id id) {
  static constexpr Spec blue_local{Id::BlueLocal, 1U, 70U, 0xF60D2E27U, 1U, 1U, nullptr};
  static constexpr Spec blue_https{Id::BlueHttps, graphics_fetch::kCacheSlot,
      graphics_fetch::kExpectedBytes, graphics_fetch::kExpectedCrc32, 1U, 1U,
      graphics_fetch::kUrl};
  static constexpr Spec color_https{Id::ColorHttps, color_asset::kSlot,
      color_asset::kBytes, color_asset::kCrc32, color_asset::kWidth,
      color_asset::kHeight, color_asset::kUrl};
  switch (id) {
    case Id::BlueLocal: return &blue_local;
    case Id::BlueHttps: return &blue_https;
    case Id::ColorHttps: return &color_https;
    default: return nullptr;
  }
}
inline const Spec *for_download(Id id) {
  const Spec *spec = find(id);
  return spec != nullptr && spec->url != nullptr ? spec : nullptr;
}
inline bool accepts(const Spec &spec, const uint8_t *png, uint32_t length) {
  cache_image::Metadata actual{};
  if (length != spec.bytes ||
      !cache_image::inspect_verified_png(png, length, spec.crc32, actual) ||
      actual.width != spec.width || actual.height != spec.height ||
      actual.bytes != spec.bytes) return false;
  // For the colored transparency fixture, check 8-bit RGBA, PNG compression,
  // filter and interlace fields as well as metadata/CRC.
  if (spec.id == Id::ColorHttps)
    return color_asset::matches_png(png, length);
  return spec.id == Id::BlueLocal || spec.id == Id::BlueHttps;
}
inline uint32_t decoded_argb_bytes(const Spec &spec) {
  const uint64_t bytes = uint64_t(spec.width) * spec.height * 4U;
  return bytes > 0U && bytes <= cache_image::kMaxPixels * 4ULL
      ? static_cast<uint32_t>(bytes) : 0U;
}
} // namespace esphome::atilla_storage::cache_catalog
