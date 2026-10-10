#pragma once
// ATILLA experimental radar-only local pixel recoloring, not production-enabled.
// RainViewer scheme 2 (Universal Blue), published dBZ mapping 0..64.
// Source: https://www.rainviewer.com/files/rainviewer_api_colors_table.csv
// Do not call on the basemap. Only call on a completed radar PNG decode.
// The map stays visible because weak return alpha is set to ZERO.
#include <cstddef>
#include <cstdint>
#include "esphome/components/online_image/online_image.h"

namespace esphome::atilla_radar_filter {
inline constexpr int kMinimumDbz = 30;
inline constexpr uint32_t kSourceRgb[65] = {0x827b69u, 0x857d6au, 0x88806cu, 0x8b826du, 0x8e856fu, 0x928871u, 0x9e9375u, 0xaa9e79u, 0xb6a97eu, 0xc2b482u, 0xcec087u, 0xd2c48bu, 0xd6c88fu, 0xdacc93u, 0xded097u, 0x88ddeeu, 0x6cd1ebu, 0x51c5e8u, 0x36bae5u, 0x1baee2u, 0x00a3e0u, 0x009ad5u, 0x0091cau, 0x0088bfu, 0x007fb4u, 0x0077aau, 0x0070a3u, 0x00699cu, 0x006295u, 0x005b8eu, 0x005588u, 0x005180u, 0x004e78u, 0x004a70u, 0x004768u, 0xffee00u, 0xffe000u, 0xffd200u, 0xffc500u, 0xffb700u, 0xffaa00u, 0xff9f00u, 0xff9500u, 0xff8b00u, 0xff8100u, 0xff4400u, 0xf23600u, 0xe62800u, 0xd91b00u, 0xcd0d00u, 0xc10000u, 0xa80000u, 0x8f0000u, 0x760000u, 0x5d0000u, 0xffaaffu, 0xff9fffu, 0xff95ffu, 0xff8bffu, 0xff81ffu, 0xff77ffu, 0xff6cffu, 0xff62ffu, 0xff58ffu, 0xff4effu};

// Custom green -> yellow -> red -> purple. Limited alpha keeps labels/coastlines visible.
inline uint32_t target_rgba(int dbz) {
  uint32_t color = 0;
  if (dbz < 35) color = 0x13bc37u;             // 30..34 green
  else if (dbz < 45) color = 0xffde00u;        // 35..44 yellow
  else if (dbz < 55) color = 0xff3528u;        // 45..54 red
  else color = 0xa934e8u;                      // 55+ purple
  return (color << 8) | (dbz < 35 ? 0x90u : 0xc8u); // moderate alpha 56%, heavy 78%
}
inline uint16_t rgb565(uint32_t c) {
  return uint16_t((((c >> 16)&0xf8u)<<8) | (((c >> 8)&0xfcu)<<3) | ((c&0xffu)>>3));
}
inline int resolve_dbz_rgb(uint32_t rgb) {
  for (int i=0;i<65;i++) if (rgb==kSourceRgb[i]) return i;
  return -1; // Never remove an unknown pixel: image may be anti-aliased.
}
inline int resolve_dbz_565(uint16_t rgb) {
  for (int i=0;i<65;i++) if (rgb==rgb565(kSourceRgb[i])) return i;
  return -1;
}

// Operates in-place on a decoded, complete tile, no second full-size buffer.
// Supported: RGB+alpha BGRA (4bpp) or RGB565+separate 8-bit alpha plane (3bpp).
// This function MUST NOT be used on chroma-key or opaque image types.
// The runtime integration must verify transparency==ALPHA_CHANNEL from YAML
// before calling; has_transparency alone does not distinguish chroma key.
inline bool apply_rain_palette(online_image::OnlineImage &img,
                               bool alpha_channel_configured,
                               bool rgb565_big_endian=false,
                               size_t *changed_out=nullptr) {
  if (changed_out) *changed_out=0;
  if (!alpha_channel_configured || !img.has_transparency() ||
      !img.is_loaded() || img.is_decoding()) return false;
  if (img.get_width()!=256 || img.get_height()!=256) return false;
  uint8_t *buf=const_cast<uint8_t*>(img.get_data_start());
  if (!buf) return false;
  const size_t count=256u*256u;
  size_t changed=0;
  if (img.get_type()==image::IMAGE_TYPE_RGB && img.get_bpp()==32) {
    for (size_t i=0;i<count;i++) {
      uint8_t *p=buf+4*i; // ESPHome BGRA.
      if (!p[3]) continue;
      const int dbz=resolve_dbz_rgb((uint32_t(p[2])<<16)|(uint32_t(p[1])<<8)|p[0]);
      if (dbz<0) continue;
      if (dbz<kMinimumDbz) { p[3]=0; }
      else {
        const uint32_t out=target_rgba(dbz);
        p[0]=(out>>8)&0xffu; p[1]=(out>>16)&0xffu; p[2]=(out>>24)&0xffu;
        p[3]=uint8_t((uint32_t(p[3])*(out&0xffu))/255u);
      }
      changed++;
    }
  } else if (img.get_type()==image::IMAGE_TYPE_RGB565 && img.get_bpp()==24) {
    uint8_t *alpha=buf+2*count;
    for (size_t i=0;i<count;i++) {
      if (!alpha[i]) continue;
      const uint16_t col=rgb565_big_endian
        ? uint16_t((uint16_t(buf[2*i])<<8)|buf[2*i+1])
        : uint16_t((uint16_t(buf[2*i+1])<<8)|buf[2*i]);
      const int dbz=resolve_dbz_565(col);
      if (dbz<0) continue;
      if (dbz<kMinimumDbz) {alpha[i]=0;}
      else {
        const uint32_t rgba=target_rgba(dbz);
        const uint16_t mapped=rgb565(rgba>>8);
        if (rgb565_big_endian) {buf[2*i]=uint8_t(mapped>>8);buf[2*i+1]=uint8_t(mapped);}
        else {buf[2*i]=uint8_t(mapped);buf[2*i+1]=uint8_t(mapped>>8);}
        alpha[i]=uint8_t((uint32_t(alpha[i])*(rgba&0xffu))/255u);
      }
      changed++;
    }
  } else return false;
  if (changed_out) *changed_out=changed;
  return true;
}
} // namespace esphome::atilla_radar_filter
