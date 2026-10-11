# Reusable read-only asset checksum primitive (development)

This branch adds `ReadOnlyFatFs::checksum_asset_readonly()` to the existing storage component without enabling any new runtime work on the panel.

The API accepts one **basename only** within `/ATILLA`, a caller-supplied size cap (at most 1 MiB), and an optional trusted expected CRC32. It rejects empty names, paths, separators, drive prefixes, leading dots, dot-dot sequences, unsupported characters and overly long filenames. It allocates its FatFs `FIL` object on the heap, opens with `FA_READ`, processes at most 256 bytes per `f_read`, calculates CRC32, closes the file on both success and failure, and returns size, checksum and chunk count. The FAT32 card remains read-only.

**Current state:** C++ API is present but not yet called by the live firmware. The previously shipped 4.7.9x combined hardware test is unchanged. Do not merge or flash this branch before 4.7.9x hardware acceptance and a follow-up integrated compile.

**Limits:** A cooperative 10-second elapsed-time guard checks between FatFs reads, but cannot interrupt a blocked read. CRC32 is appropriate for detecting accidental corruption, not cryptographic tampering. This is not a manifest loader, image renderer, general directory traversal or storage write service. Separately supplied expected CRC values must be obtained from trusted asset build metadata.

**Next grouped acceptance:** After 4.7.9x hardware PASS, adapt the combined SD worker to exercise a known asset via this reusable function and validate expected CRC32, with one full firmware upload and one startup log rather than multiple separate test cycles.
