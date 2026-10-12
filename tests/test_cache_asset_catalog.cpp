#include <cassert>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <vector>
#include "../components/atilla_storage/cache_asset_catalog.h"
using namespace esphome::atilla_storage;

static std::vector<uint8_t> load(const char *path) {
  std::ifstream f(path, std::ios::binary);
  assert(f.good());
  return std::vector<uint8_t>(std::istreambuf_iterator<char>{f},
                              std::istreambuf_iterator<char>{});
}
int main() {
  using cache_catalog::Id;
  const auto *local = cache_catalog::find(Id::BlueLocal);
  const auto *blue = cache_catalog::find(Id::BlueHttps);
  const auto *color = cache_catalog::find(Id::ColorHttps);
  assert(local && blue && color);
  assert(cache_catalog::find(static_cast<Id>(0)) == nullptr);
  assert(cache_catalog::find(static_cast<Id>(255)) == nullptr);
  assert(cache_catalog::for_download(Id::BlueLocal) == nullptr);
  assert(cache_catalog::for_download(Id::BlueHttps) == blue);
  assert(cache_catalog::for_download(Id::ColorHttps) == color);
  assert(local->slot == 1 && blue->slot == 2 && color->slot == 3);
  assert(local->url == nullptr);
  assert(blue->url && color->url);
  assert(cache_catalog::decoded_argb_bytes(*local) == 4u);
  assert(cache_catalog::decoded_argb_bytes(*blue) == 4u);
  assert(cache_catalog::decoded_argb_bytes(*color) == 24576u);

  auto pixel = load("assets/test/cache-test-1x1.png");
  auto badge = load("assets/test/sd-color-preview-96x64.png");
  assert(pixel.size() == blue->bytes && badge.size() == color->bytes);
  assert(cache_catalog::accepts(*local, pixel.data(), uint32_t(pixel.size())));
  assert(cache_catalog::accepts(*blue, pixel.data(), uint32_t(pixel.size())));
  assert(!cache_catalog::accepts(*color, pixel.data(), uint32_t(pixel.size())));
  assert(cache_catalog::accepts(*color, badge.data(), uint32_t(badge.size())));
  assert(!cache_catalog::accepts(*blue, badge.data(), uint32_t(badge.size())));
  assert(!cache_catalog::accepts(*color, badge.data(), uint32_t(badge.size()-1)));
  assert(!cache_catalog::accepts(*blue, nullptr, blue->bytes));
  assert(!cache_catalog::accepts(*color, nullptr, color->bytes));
  for (size_t i : {size_t(0), size_t(19), size_t(25), size_t(40), badge.size()-13}) {
    auto corrupted = badge;
    corrupted[i] ^= 1u;
    assert(!cache_catalog::accepts(*color, corrupted.data(), uint32_t(corrupted.size())));
  }
  auto corrupted = pixel;
  corrupted[43] ^= 1u;
  assert(!cache_catalog::accepts(*blue, corrupted.data(), uint32_t(corrupted.size())));
  return 0;
}
