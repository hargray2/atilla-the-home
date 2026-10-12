#include <cassert>
#include <cstdint>
#include <cstring>
#include "../components/atilla_storage/cache_image_contract.h"
using namespace esphome::atilla_storage::cache_image;
static constexpr uint8_t kOnePixelPng[70] = {
  0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,
  0x49,0x48,0x44,0x52,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,
  0x08,0x06,0x00,0x00,0x00,0x1f,0x15,0xc4,0x89,0x00,0x00,0x00,
  0x0d,0x49,0x44,0x41,0x54,0x78,0x9c,0x63,0x90,0xc9,0x5f,
  0xf2,0x1f,0x00,0x04,0x09,0x02,0x2f,0x43,0x1f,0x6a,0x9a,
  0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82
};
int main() {
  // Fail-closed handoff: no worker payload may escape until successful
  // FatFs unmount, including the failure case with an otherwise valid PNG.
  for (int read_mode = 0; read_mode < 2; ++read_mode) {
    for (int unmounted = 0; unmounted < 2; ++unmounted) {
      for (int owned = 0; owned < 2; ++owned) {
        const bool allowed = read_mode && unmounted && owned;
        assert(handoff_ready(read_mode, unmounted, owned) == allowed);
      }
    }
  }
  assert(valid_request(1, 70));
  assert(valid_request(9999, 262144));
  assert(!valid_request(0, 70));
  assert(!valid_request(10000, 70));
  assert(!valid_request(1, 0));
  assert(!valid_request(1, 262145));
  assert(!valid_request(1, 44));
  assert(crc32(kOnePixelPng, sizeof(kOnePixelPng)) == 0xF60D2E27U);
  Metadata meta{};
  assert(inspect_verified_png(kOnePixelPng, sizeof(kOnePixelPng), 0xF60D2E27U, meta));
  assert(meta.width == 1 && meta.height == 1 && meta.bytes == 70);
  assert(meta.crc32 == 0xF60D2E27U);
  assert(!inspect_verified_png(kOnePixelPng, sizeof(kOnePixelPng), 0, meta));
  assert(meta.bytes == 0);
  assert(!inspect_verified_png(nullptr, 70, 0xF60D2E27U, meta));
  uint8_t invalid[sizeof(kOnePixelPng)];
  std::memcpy(invalid, kOnePixelPng, sizeof(invalid));
  invalid[0] = 0;
  assert(!inspect_verified_png(invalid, sizeof(invalid), 0xF60D2E27U, meta));
  std::memcpy(invalid, kOnePixelPng, sizeof(invalid));
  invalid[19] = 0;  // Zero-width image is invalid.
  assert(!inspect_verified_png(invalid, sizeof(invalid), 0xF60D2E27U, meta));
  std::memcpy(invalid, kOnePixelPng, sizeof(invalid));
  invalid[65] = 0;  // Invalid fixed IEND trailer.
  assert(!inspect_verified_png(invalid, sizeof(invalid), 0xF60D2E27U, meta));
  std::memcpy(invalid, kOnePixelPng, sizeof(invalid));
  invalid[43] ^= 1;  // CRC corruption, even though PNG envelope still looks valid.
  assert(!inspect_verified_png(invalid, sizeof(invalid), 0xF60D2E27U, meta));
  return 0;
}
