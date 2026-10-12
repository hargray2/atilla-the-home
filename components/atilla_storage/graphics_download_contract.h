#pragma once
// First-generation network staging policy. No user-supplied URL accepted.
// Immutable public test resource, separate from all private weather and radar URLs.
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace esphome::atilla_storage::graphics_fetch {
constexpr const char *kUrl =
    "https://raw.githubusercontent.com/hargray2/atilla-the-home/"
    "0bcaabe0919e0311dbd8c93a9dfa664b8a25bde2/assets/test/cache-test-1x1.png";
constexpr uint32_t kExpectedBytes = 70;
constexpr uint32_t kExpectedCrc32 = 0xF60D2E27U;
constexpr unsigned kCacheSlot = 2;
constexpr uint32_t kMaxNetworkAssetBytes = 1048576U;
inline bool approved_url(const char *url) {
  return url != nullptr && std::strcmp(url, kUrl) == 0;
}
inline bool png_header(const uint8_t *data, size_t size) {
  static constexpr uint8_t sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
  if (!data || size < 33) return false;
  for (unsigned i = 0; i < 8; ++i)
    if (data[i] != sig[i]) return false;
  // A PNG must start with a 13-byte IHDR defining bounded pixel dimensions.
  if (data[8] || data[9] || data[10] || data[11] != 13 ||
      data[12] != 'I' || data[13] != 'H' || data[14] != 'D' || data[15] != 'R')
    return false;
  const uint32_t width = (uint32_t(data[16]) << 24) | (uint32_t(data[17]) << 16) |
                         (uint32_t(data[18]) << 8) | data[19];
  const uint32_t height = (uint32_t(data[20]) << 24) | (uint32_t(data[21]) << 16) |
                          (uint32_t(data[22]) << 8) | data[23];
  return width > 0 && width <= 800 && height > 0 && height <= 480;
}
inline bool png_trailer(const uint8_t *data, size_t size) {
  static constexpr uint8_t iend[8] = {0,0,0,0, 'I','E','N','D'};
  static constexpr uint8_t crc[4] = {0xAE,0x42,0x60,0x82};
  if (!data || size < 45) return false;
  for (unsigned i=0; i<8; ++i)
    if (data[size-12+i] != iend[i]) return false;
  for (unsigned i=0; i<4; ++i)
    if (data[size-4+i] != crc[i]) return false;
  return true;
}
class BoundedReceiver {
 public:
  BoundedReceiver(uint8_t *target, uint32_t expected_len,
                  uint32_t expected_crc)
      : target_(target), expected_len_(expected_len), expected_crc_(expected_crc) {}
  bool begin(int http_status, int64_t header_content_length, bool chunked) {
    if (!target_ || http_status != 200 || chunked ||
        expected_len_ == 0 || expected_len_ > kMaxNetworkAssetBytes ||
        header_content_length != expected_len_) return false;
    active_ = true;
    return true;
  }
  bool feed(const uint8_t *data, size_t length) {
    if (!active_ || !data || length == 0 ||
        length > expected_len_ - received_) return false;
    std::memcpy(target_ + received_, data, length);
    for (size_t i = 0; i < length; ++i) {
      crc_ ^= data[i];
      for (unsigned bit = 0; bit < 8; ++bit)
        crc_ = (crc_ >> 1) ^ ((crc_ & 1U) ? 0xedb88320U : 0U);
    }
    received_ += length;
    return true;
  }
  bool complete() const {
    return active_ && received_ == expected_len_ &&
           (crc_ ^ 0xffffffffU) == expected_crc_ &&
           png_header(target_, received_) && png_trailer(target_, received_);
  }
  uint32_t received() const { return received_; }
 private:
  uint8_t *target_{nullptr};
  uint32_t expected_len_{0}, expected_crc_{0}, received_{0};
  uint32_t crc_{0xffffffffU};
  bool active_{false};
};
}  // namespace esphome::atilla_storage::graphics_fetch
