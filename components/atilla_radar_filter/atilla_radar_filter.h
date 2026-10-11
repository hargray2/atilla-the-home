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
  // Exact RGB565 palette index: bounded switch instead of 65 comparisons/pixel.
  switch (rgb) {
    case 0x83cdu: return 0;
    case 0x83edu: return 1;
    case 0x8c0du: return 3;
    case 0x8c2du: return 4;
    case 0x944eu: return 5;
    case 0x9c8eu: return 6;
    case 0xacefu: return 7;
    case 0xb54fu: return 8;
    case 0xc5b0u: return 9;
    case 0xce10u: return 10;
    case 0xd631u: return 11;
    case 0xd651u: return 12;
    case 0xde72u: return 13;
    case 0xde92u: return 14;
    case 0x8efdu: return 15;
    case 0x6e9du: return 16;
    case 0x563du: return 17;
    case 0x35dcu: return 18;
    case 0x1d7cu: return 19;
    case 0x051cu: return 20;
    case 0x04dau: return 21;
    case 0x0499u: return 22;
    case 0x0457u: return 23;
    case 0x03f6u: return 24;
    case 0x03b5u: return 25;
    case 0x0394u: return 26;
    case 0x0353u: return 27;
    case 0x0312u: return 28;
    case 0x02d1u: return 29;
    case 0x02b1u: return 30;
    case 0x0290u: return 31;
    case 0x026fu: return 32;
    case 0x024eu: return 33;
    case 0x022du: return 34;
    case 0xff60u: return 35;
    case 0xff00u: return 36;
    case 0xfe80u: return 37;
    case 0xfe20u: return 38;
    case 0xfda0u: return 39;
    case 0xfd40u: return 40;
    case 0xfce0u: return 41;
    case 0xfca0u: return 42;
    case 0xfc40u: return 43;
    case 0xfc00u: return 44;
    case 0xfa20u: return 45;
    case 0xf1a0u: return 46;
    case 0xe140u: return 47;
    case 0xd8c0u: return 48;
    case 0xc860u: return 49;
    case 0xc000u: return 50;
    case 0xa800u: return 51;
    case 0x8800u: return 52;
    case 0x7000u: return 53;
    case 0x5800u: return 54;
    case 0xfd5fu: return 55;
    case 0xfcffu: return 56;
    case 0xfcbfu: return 57;
    case 0xfc5fu: return 58;
    case 0xfc1fu: return 59;
    case 0xfbbfu: return 60;
    case 0xfb7fu: return 61;
    case 0xfb1fu: return 62;
    case 0xfadfu: return 63;
    case 0xfa7fu: return 64;
    case 0x630bu: return -10;
    case 0x6b2bu: return -10;
    case 0x6b4bu: return -10;
    case 0x736cu: return -10;
    case 0x738cu: return -10;
    case 0x7b8cu: return -10;
    case 0x7bacu: return -10;
    case 0x7bccu: return -10;
    case 0xffffu: return 65; // RainViewer >=65 dBZ rendered white; treat as extreme precipitation.
    default: return -1;
  }
}

// Operates in-place on a decoded, complete tile, no second full-size buffer.
// Supported: RGB+alpha BGRA (4bpp) or RGB565+separate 8-bit alpha plane (get_bpp()==16; 3 bytes/pixel allocated).
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
  } else if (img.get_type()==image::IMAGE_TYPE_RGB565 && img.get_bpp()==16) {
    uint8_t *alpha=buf+2*count;
    for (size_t i=0;i<count;i++) {
      if (!alpha[i]) continue;
      const uint16_t col=rgb565_big_endian
        ? uint16_t((uint16_t(buf[2*i])<<8)|buf[2*i+1])
        : uint16_t((uint16_t(buf[2*i+1])<<8)|buf[2*i]);
      const int dbz=resolve_dbz_565(col);
      if (dbz==-1) continue;
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

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/core/log.h"
namespace esphome::atilla_radar_filter {
class RadarFilter : public Component {
 public:
  void setup() override {}
  void dump_config() override { ESP_LOGCONFIG("atilla_radar_filter", "ATILLA radar filter: hide <30 dBZ, green/yellow/red/purple"); }
  void apply(online_image::OnlineImage *img) {
    if (img == nullptr) return;
    size_t changed=0;
    // Only use with online_image configured type RGB + transparency alpha_channel.
    if (!apply_rain_palette(*img, true, false, &changed)) {
      ESP_LOGW("atilla_radar_filter", "Skipped incompatible or unfinished radar tile");
    } else {
      ESP_LOGI("atilla_radar_filter", "Radar palette processed %u pixels", unsigned(changed));
    }
  }
};
template<typename... Ts> class FilterAction : public Action<Ts...> {
 public:
  FilterAction(RadarFilter *owner, online_image::OnlineImage *img) : owner_(owner), img_(img) {}
  void play(const Ts &... x) override { owner_->apply(img_); }
 private:
  RadarFilter *owner_;
  online_image::OnlineImage *img_;
};
}  // namespace esphome::atilla_radar_filter
