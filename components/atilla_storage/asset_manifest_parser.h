#pragma once
// Minimal allocation-free parser for the bounded ATILLA manifest contract.
// Deliberately rejects escapes, floats, duplicate keys and unsupported members.
// Caller must separately verify asset file length and CRC on SD.
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace esphome::atilla_storage::manifest {
struct Entry {
  char path[97]{};
  char kind[11]{};
  uint32_t bytes{0};
  uint32_t crc32{0};
};
struct Document { Entry assets[32]{}; unsigned count{0}; };

class Parser {
 public:
  Parser(const char *data, size_t size) : data_(data), size_(size) {}
  bool parse(Document &out) {
    out = Document{};
    if (!data_ || size_ == 0 || size_ > 16384) return false;
    if (!expect_('{')) return false;
    bool seen_schema = false, seen_assets = false;
    if (peek_('}')) return false;
    do {
      char key[24];
      if (!string_(key, sizeof(key)) || !expect_(':')) return false;
      if (std::strcmp(key, "schema_version") == 0 && !seen_schema) {
        uint32_t n;
        if (!uint_(n) || n != 1) return false;
        seen_schema = true;
      } else if (std::strcmp(key, "assets") == 0 && !seen_assets) {
        if (!array_(out)) return false;
        seen_assets = true;
      } else return false;
      if (peek_('}')) break;
    } while (expect_(','));
    return seen_schema && seen_assets && expect_('}') && done_();
  }

 private:
  void ws_() {
    while (pos_ < size_ && (data_[pos_] == ' ' || data_[pos_] == '\n' ||
                            data_[pos_] == '\r' || data_[pos_] == '\t')) ++pos_;
  }
  bool peek_(char c) { ws_(); return pos_ < size_ && data_[pos_] == c; }
  bool expect_(char c) {
    if (!peek_(c)) return false;
    ++pos_;
    return true;
  }
  bool done_() { ws_(); return pos_ == size_; }
  bool string_(char *out, size_t cap) {
    if (!out || cap == 0 || !expect_('"')) return false;
    size_t length = 0;
    while (pos_ < size_) {
      const unsigned char c = static_cast<unsigned char>(data_[pos_++]);
      if (c == '"') {
        if (length == 0) return false;
        out[length] = 0;
        return true;
      }
      if (c < 0x20 || c >= 0x7f || c == '\\' || length + 1 >= cap) return false;
      out[length++] = static_cast<char>(c);
    }
    return false;
  }
  bool uint_(uint32_t &n) {
    ws_();
    if (pos_ == size_ || data_[pos_] < '0' || data_[pos_] > '9') return false;
    if (data_[pos_] == '0' && pos_ + 1 < size_ &&
        data_[pos_ + 1] >= '0' && data_[pos_ + 1] <= '9') return false;
    uint32_t v = 0;
    while (pos_ < size_ && data_[pos_] >= '0' && data_[pos_] <= '9') {
      const uint32_t digit = data_[pos_++] - '0';
      if (v > (UINT32_MAX - digit) / 10) return false;
      v = v * 10 + digit;
    }
    n = v;
    return true;
  }
  static char lower_(char c) { return c >= 'A' && c <= 'Z' ? c + ('a' - 'A') : c; }
  static bool safe_path_(const char *p) {
    const size_t len = std::strlen(p);
    if (len == 0 || len > 96 || p[0] == '.' || p[0] == '/') return false;
    unsigned depth = 1;
    for (size_t i = 0; i < len; ++i) {
      const char c = p[i];
      const bool safe = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
          (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.' || c == '/';
      if (!safe) return false;
      if (c == '/') {
        if (i == 0 || i + 1 == len || p[i + 1] == '/' || p[i + 1] == '.' ||
            p[i - 1] == '.') return false;
        if (++depth > 4) return false;
      }
      if (c == '.' && i + 1 < len && p[i + 1] == '.') return false;
    }
    return true;
  }
  static bool extension_(const Entry &e) {
    const char *ext = std::strrchr(e.path, '.');
    if (!ext) return false;
    char suffix[8]{}; size_t i = 0;
    for (++ext; *ext && i < sizeof(suffix) - 1; ++ext) suffix[i++] = lower_(*ext);
    if (*ext) return false;
    const bool png = std::strcmp(suffix, "png") == 0;
    const bool jpg = std::strcmp(suffix, "jpg") == 0 || std::strcmp(suffix, "jpeg") == 0;
    return (png && std::strcmp(e.kind, "image/png") == 0) ||
           (jpg && std::strcmp(e.kind, "image/jpeg") == 0);
  }
  bool hex_(uint32_t &v) {
    char str[9];
    if (!string_(str, sizeof(str)) || std::strlen(str) != 8) return false;
    v = 0;
    for (unsigned i = 0; i < 8; ++i) {
      char c = str[i]; unsigned n;
      if (c >= '0' && c <= '9') n = c - '0';
      else if (c >= 'a' && c <= 'f') n = c - 'a' + 10;
      else if (c >= 'A' && c <= 'F') n = c - 'A' + 10;
      else return false;
      v = (v << 4) | n;
    }
    return true;
  }
  bool entry_(Entry &e) {
    if (!expect_('{') || peek_('}')) return false;
    unsigned seen = 0;
    do {
      char key[24];
      if (!string_(key, sizeof(key)) || !expect_(':')) return false;
      unsigned bit = 0;
      if (std::strcmp(key, "path") == 0) {
        bit = 1;
        if (!string_(e.path, sizeof(e.path)) || !safe_path_(e.path)) return false;
      } else if (std::strcmp(key, "kind") == 0) {
        bit = 2;
        if (!string_(e.kind, sizeof(e.kind))) return false;
      } else if (std::strcmp(key, "bytes") == 0) {
        bit = 4;
        if (!uint_(e.bytes) || e.bytes == 0 || e.bytes > 1048576) return false;
      } else if (std::strcmp(key, "crc32") == 0) {
        bit = 8;
        if (!hex_(e.crc32)) return false;
      } else return false;
      if (seen & bit) return false;
      seen |= bit;
      if (peek_('}')) break;
    } while (expect_(','));
    return seen == 15 && expect_('}') && extension_(e);
  }
  static bool equal_ci_(const char *a, const char *b) {
    for (; *a && *b; ++a, ++b) if (lower_(*a) != lower_(*b)) return false;
    return *a == *b;
  }
  bool array_(Document &out) {
    if (!expect_('[')) return false;
    if (peek_(']')) return expect_(']');
    do {
      if (out.count >= 32 || !entry_(out.assets[out.count])) return false;
      for (unsigned j = 0; j < out.count; ++j)
        if (equal_ci_(out.assets[j].path, out.assets[out.count].path)) return false;
      ++out.count;
      if (peek_(']')) break;
    } while (expect_(','));
    return expect_(']');
  }
  const char *data_;
  size_t size_;
  size_t pos_{0};
};
}  // namespace esphome::atilla_storage::manifest
