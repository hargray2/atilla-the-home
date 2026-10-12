#include <cassert>
#include <cstring>
#include "../components/atilla_storage/asset_manifest_parser.h"
using esphome::atilla_storage::manifest::Document;
using esphome::atilla_storage::manifest::Parser;
static bool parse(const char *s) {
  Document d;
  return Parser(s, std::strlen(s)).parse(d);
}
int main() {
  const char *good = R"({"schema_version":1,"assets":[{"path":"icons/atilla-test.png","kind":"image/png","bytes":70,"crc32":"5ACE03B3"}]})";
  Document d;
  assert(Parser(good, std::strlen(good)).parse(d));
  assert(d.count == 1 && d.assets[0].bytes == 70 && d.assets[0].crc32 == 0x5ACE03B3U);
  assert(parse(R"({"assets":[],"schema_version":1})"));
  assert(!parse(R"({"schema_version":1,"assets":[],"assets":[]})"));
  assert(!parse(R"({"schema_version":1,"assets":[{"path":"../x.png","kind":"image/png","bytes":70,"crc32":"00000000"}]})"));
  assert(!parse(R"({"schema_version":1,"assets":[{"path":"icons/a.jpg","kind":"image/png","bytes":70,"crc32":"00000000"}]})"));
  assert(!parse(R"({"schema_version":1,"assets":[{"path":"icons/a.png","kind":"image/png","bytes":0,"crc32":"00000000"}]})"));
  assert(!parse(R"({"schema_version":1,"assets":[{"path":"icons/a.png","kind":"image/png","bytes":70,"crc32":"bad"}]})"));
  assert(!parse(R"({"schema_version":1,"assets":[{"path":"icons/a.png","kind":"image/png","bytes":70,"crc32":"00000000"},{"path":"ICONS/A.PNG","kind":"image/png","bytes":70,"crc32":"00000000"}]})"));
  assert(!parse(R"({"schema_version":1,"assets":[],"extra":true})"));
  return 0;
}
