#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <vector>
#include "../components/atilla_storage/color_asset_contract.h"
#include "../components/atilla_storage/cache_image_contract.h"
using namespace esphome::atilla_storage;
int main() {
  static_assert(color_asset::kSlot == 3U);
  static_assert(color_asset::kMaxDecodedArgbBytes == 24576U);
  assert(color_asset::approved_url(color_asset::kUrl));
  assert(!color_asset::approved_url(nullptr));
  assert(!color_asset::approved_url(graphics_fetch::kUrl));
  assert(!color_asset::approved_url("https://example.org/image.png"));
  std::ifstream file("assets/test/sd-color-preview-96x64.png", std::ios::binary);
  assert(file.good());
  const std::vector<uint8_t> png(std::istreambuf_iterator<char>{file},
                                 std::istreambuf_iterator<char>{});
  assert(png.size() == color_asset::kBytes);
  assert(color_asset::matches_png(png.data(), static_cast<uint32_t>(png.size())));
  cache_image::Metadata md{};
  assert(cache_image::inspect_verified_png(png.data(),
      static_cast<uint32_t>(png.size()), color_asset::kCrc32, md));
  assert(md.width == color_asset::kWidth && md.height == color_asset::kHeight);
  assert(md.bytes == color_asset::kBytes);
  std::vector<uint8_t> output(png.size(), 0);
  graphics_fetch::BoundedReceiver receiver(output.data(), color_asset::kBytes,
                                            color_asset::kCrc32);
  assert(!receiver.begin(302, png.size(), false));
  assert(receiver.begin(200, png.size(), false));
  size_t offset = 0;
  while (offset < png.size()) {
    const size_t take = std::min<size_t>(23, png.size() - offset);
    assert(receiver.feed(png.data() + offset, take));
    offset += take;
  }
  assert(receiver.complete());
  assert(output == png);
  for (size_t pos : {size_t(0), size_t(19), size_t(25), size_t(40), png.size()-13}) {
    auto corrupt = png;
    corrupt[pos] ^= 1;
    assert(!color_asset::matches_png(corrupt.data(),
                                     static_cast<uint32_t>(corrupt.size())));
  }
  assert(!color_asset::matches_png(png.data(), uint32_t(png.size() - 1)));
  assert(!color_asset::matches_png(nullptr, color_asset::kBytes));
  return 0;
}
