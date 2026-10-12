#pragma once
// Safe, fixed-name immutable graphics-cache transaction contract.
// Do not accept an arbitrary caller-supplied FatFs path for writable assets.
#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace esphome::atilla_storage::cache {
constexpr uint32_t kMaxAssetBytes = 1048576U;
constexpr unsigned kFirstSlot = 1;
constexpr unsigned kLastSlot = 9999;
struct Paths {
  char directory[32]{};
  char pending[64]{};
  char committed[64]{};
};
inline bool make_paths(const char *drive, unsigned slot, Paths &out) {
  out = Paths{};
  if (!drive || slot < kFirstSlot || slot > kLastSlot) return false;
  // The drive comes only from the FatFs-assigned numeric drive ID, e.g. "0:".
  if (drive[0] < '0' || drive[0] > '9' || drive[1] != ':' || drive[2] != '\0') return false;
  const int d = std::snprintf(out.directory, sizeof(out.directory), "%s/ATILLA/CACHE", drive);
  const int p = std::snprintf(out.pending, sizeof(out.pending),
                              "%s/ATILLA/CACHE/C%04u.TMP", drive, slot);
  const int c = std::snprintf(out.committed, sizeof(out.committed),
                              "%s/ATILLA/CACHE/C%04u.PNG", drive, slot);
  return d > 0 && static_cast<size_t>(d) < sizeof(out.directory) &&
         p > 0 && static_cast<size_t>(p) < sizeof(out.pending) &&
         c > 0 && static_cast<size_t>(c) < sizeof(out.committed);
}
inline uint32_t crc32(const uint8_t *data, size_t size) {
  if (!data || size == 0 || size > kMaxAssetBytes) return 0;
  uint32_t crc = 0xffffffffU;
  for (size_t i = 0; i < size; ++i) {
    crc ^= data[i];
    for (unsigned b = 0; b < 8; ++b)
      crc = (crc >> 1) ^ ((crc & 1U) ? 0xedb88320U : 0U);
  }
  return crc ^ 0xffffffffU;
}
inline bool is_png(const uint8_t *data, size_t size) {
  static constexpr uint8_t signature[8] = {137,80,78,71,13,10,26,10};
  if (!data || size < 20 || size > kMaxAssetBytes) return false;
  for (unsigned i = 0; i < 8; ++i) if (data[i] != signature[i]) return false;
  // A signature is a cheap guard only, NOT complete PNG stream validation.
  return true;
}
}  // namespace esphome::atilla_storage::cache
