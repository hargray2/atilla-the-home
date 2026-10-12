#include <cassert>
#include <cstdint>
#include <cstring>
#include "../components/atilla_storage/graphics_download_contract.h"
using namespace esphome::atilla_storage::graphics_fetch;

static constexpr uint8_t kFixture[70] = {
  0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,
  0x49,0x48,0x44,0x52,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,
  0x08,0x06,0x00,0x00,0x00,0x1f,0x15,0xc4,0x89,0x00,0x00,0x00,
  0x0d,0x49,0x44,0x41,0x54,0x78,0x9c,0x63,0x90,0xc9,0x5f,
  0xf2,0x1f,0x00,0x04,0x09,0x02,0x2f,0x43,0x1f,0x6a,0x9a,
  0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82
};

int main() {
  assert(!tls_clock_ready(0));
  assert(!tls_clock_ready(1767225599LL));
  assert(tls_clock_ready(1767225600LL));
  assert(tls_clock_ready(1791763200LL));
  assert(approved_url(kUrl));
  assert(!approved_url(nullptr));
  assert(!approved_url("http://raw.githubusercontent.com/example.png"));
  assert(!approved_url("https://raw.githubusercontent.com/other/path"));
  assert(kCacheSlot != 1); // Never overwrite first hardware-validated slot.
  assert(kExpectedBytes == sizeof(kFixture));
  assert(png_header(kFixture, sizeof(kFixture)));
  assert(png_trailer(kFixture, sizeof(kFixture)));

  uint8_t target[sizeof(kFixture)]{};
  BoundedReceiver valid(target, sizeof(target), kExpectedCrc32);
  assert(valid.begin(200, sizeof(target), false));
  assert(valid.feed(kFixture, 11));
  assert(valid.feed(kFixture + 11, 15));
  assert(valid.feed(kFixture + 26, sizeof(kFixture) - 26));
  assert(valid.received() == sizeof(kFixture));
  assert(valid.complete());
  assert(std::memcmp(target, kFixture, sizeof(target)) == 0);
  assert(!valid.feed(kFixture, 1));

  BoundedReceiver redirect(target, sizeof(target), kExpectedCrc32);
  assert(!redirect.begin(302, sizeof(target), false));
  assert(!redirect.feed(kFixture, 1));
  BoundedReceiver not_found(target, sizeof(target), kExpectedCrc32);
  assert(!not_found.begin(404, sizeof(target), false));
  BoundedReceiver chunked(target, sizeof(target), kExpectedCrc32);
  assert(!chunked.begin(200, -1, true));
  BoundedReceiver unknown_length(target, sizeof(target), kExpectedCrc32);
  assert(!unknown_length.begin(200, -1, false));
  BoundedReceiver truncated(target, sizeof(target), kExpectedCrc32);
  assert(truncated.begin(200, sizeof(target), false));
  assert(truncated.feed(kFixture, sizeof(kFixture) - 1));
  assert(!truncated.complete());
  BoundedReceiver oversized(target, sizeof(target), kExpectedCrc32);
  assert(oversized.begin(200, sizeof(target), false));
  assert(!oversized.feed(kFixture, sizeof(kFixture) + 1));
  assert(!oversized.complete());
  BoundedReceiver bad_crc(target, sizeof(target), kExpectedCrc32);
  assert(bad_crc.begin(200, sizeof(target), false));
  uint8_t bad[sizeof(kFixture)];
  std::memcpy(bad, kFixture, sizeof(bad));
  bad[43] ^= 1;
  assert(bad_crc.feed(bad, sizeof(bad)));
  assert(!bad_crc.complete());
  return 0;
}
