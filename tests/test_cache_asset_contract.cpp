#include <cassert>
#include <cstring>
#include "../components/atilla_storage/cache_asset_contract.h"
using esphome::atilla_storage::cache::Paths;
using esphome::atilla_storage::cache::make_paths;
using esphome::atilla_storage::cache::crc32;
using esphome::atilla_storage::cache::is_png;
int main() {
  Paths a;
  assert(make_paths("0:", 1, a));
  assert(std::strcmp(a.directory, "0:/ATILLA/CACHE") == 0);
  assert(std::strcmp(a.pending, "0:/ATILLA/CACHE/C0001.TMP") == 0);
  assert(std::strcmp(a.committed, "0:/ATILLA/CACHE/C0001.PNG") == 0);
  assert(make_paths("9:", 9999, a));
  assert(std::strcmp(a.pending, "9:/ATILLA/CACHE/C9999.TMP") == 0);
  assert(!make_paths("0:", 0, a));
  assert(!make_paths("0:", 10000, a));
  assert(!make_paths("../../", 1, a));
  assert(!make_paths("10:", 1, a));
  assert(!make_paths("0:/", 1, a));
  static constexpr uint8_t png[] = {
    0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,
    0x49,0x48,0x44,0x52,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,
    0x08,0x06,0x00,0x00,0x00,0x1f,0x15,0xc4,0x89,0x00,0x00,0x00,
    0x0d,0x49,0x44,0x41,0x54,0x78,0x9c,0x63,0x90,0xc9,0x5f,
    0xf2,0x1f,0x00,0x04,0x09,0x02,0x2f,0x43,0x1f,0x6a,0x9a,
    0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82
  };
  assert(sizeof(png) == 70);
  assert(is_png(png, sizeof(png)));
  assert(crc32(png, sizeof(png)) == 0xF60D2E27U);
  assert(!is_png(png + 1, sizeof(png) - 1));
  assert(!is_png(png, 10));
  assert(!is_png(nullptr, sizeof(png)));
  assert(crc32(nullptr, 4) == 0);
  return 0;
}
