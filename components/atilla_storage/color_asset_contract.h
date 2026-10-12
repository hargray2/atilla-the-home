#pragma once
// One fixed, immutable multicolor RGBA PNG. No arbitrary host/URL, slot or path.
#include <cstdint>
#include <cstring>
#include "graphics_download_contract.h"
namespace esphome::atilla_storage::color_asset {
constexpr const char *kUrl =
    "https://raw.githubusercontent.com/hargray2/atilla-the-home/"
    "dbb2c26e5103b55c44bd5f0e0003523dd035a7e9/assets/test/sd-color-preview-96x64.png";
constexpr unsigned kSlot = 3U;
constexpr uint32_t kBytes = 2314U;
constexpr uint32_t kCrc32 = 0xB171217CU;
constexpr uint16_t kWidth = 96U, kHeight = 64U;
constexpr uint32_t kMaxDecodedArgbBytes = uint32_t(kWidth) * kHeight * 4U;
inline bool approved_url(const char *url) {
  return url != nullptr && std::strcmp(url, kUrl) == 0;
}
inline bool matches_png(const uint8_t *data, uint32_t size) {
  if (!data || size != kBytes ||
      !graphics_fetch::png_header(data, size) ||
      !graphics_fetch::png_trailer(data, size)) return false;
  const auto be32=[](const uint8_t *p)->uint32_t {
    return uint32_t(p[0])<<24 | uint32_t(p[1])<<16 |
           uint32_t(p[2])<<8 | uint32_t(p[3]);
  };
  // RGBA/8-bit, compression/filter method 0, non-interlaced PNG. The decoder
  // must still validate the IDAT stream; this is a fixed-envelope preflight.
  if (be32(data + 16) != kWidth || be32(data + 20) != kHeight ||
      data[24] != 8 || data[25] != 6 || data[26] != 0 ||
      data[27] != 0 || data[28] != 0) return false;
  uint32_t crc = 0xFFFFFFFFu;
  for(uint32_t i=0;i<size;++i) {
    crc ^= data[i];
    for(unsigned bit=0;bit<8;++bit)
      crc = (crc>>1) ^ ((crc&1u)?0xEDB88320u:0u);
  }
  return (crc ^ 0xFFFFFFFFu) == kCrc32;
}
} // namespace esphome::atilla_storage::color_asset
