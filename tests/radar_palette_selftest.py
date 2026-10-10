"""Static regression checks for RainViewer scheme 2 pixel mapping.
These tests do not require private YAML or Waveshare hardware.
"""
from pathlib import Path
import re

HEADER = Path("components/atilla_radar_filter/atilla_radar_filter.h").read_text()
PALETTE = re.search(r"kSourceRgb\[65\] = \{([^}]*)\}", HEADER).group(1)
COLORS = [int(x, 16) for x in re.findall(r"0x([0-9a-fA-F]+)u", PALETTE)]
assert len(COLORS) == 65
assert COLORS[0] == 0x827B69   # 0 dBZ
assert COLORS[29] == 0x005B8E  # highest blue echo suppressed
assert COLORS[30] == 0x005588  # first kept echo, becomes green
assert COLORS[35] == 0xFFEE00  # yellow cutoff
assert COLORS[45] == 0xFF4400  # red cutoff
assert COLORS[55] == 0xFFAAFF  # purple cutoff
assert COLORS[64] == 0xFF4EFF
assert "kMinimumDbz = 30" in HEADER
assert "if (dbz<kMinimumDbz) {alpha[i]=0;}" in HEADER
assert "img.get_bpp()==16" in HEADER
assert "case 0xffffu: return 65;" in HEADER
assert "return -10;" in HEADER

def rgb565(rgb):
    return (((rgb >> 16) & 0xF8) << 8) | (((rgb >> 8) & 0xFC) << 3) | ((rgb & 0xFF) >> 3)

section = HEADER.split("inline int resolve_dbz_565(", 1)[1].split("// Operates in-place", 1)[0]
cases = dict((int(c, 16), int(dbz)) for c, dbz in re.findall(r"case 0x([0-9a-fA-F]+)u: return (-?\d+);", section))
for dbz, rgb in enumerate(COLORS):
    actual = cases[rgb565(rgb)]
    assert actual == dbz or (dbz < 30 and actual < 30), (dbz, actual)
assert cases[0xffff] == 65

def render(dbz):
    if dbz < 30:
        return "transparent"
    if dbz < 35:
        return "green"
    if dbz < 45:
        return "yellow"
    if dbz < 55:
        return "red"
    return "purple"

for input_dbz, expected in [(-10, "transparent"), (0, "transparent"),
                            (29, "transparent"), (30, "green"),
                            (34, "green"), (35, "yellow"),
                            (44, "yellow"), (45, "red"),
                            (54, "red"), (55, "purple"),
                            (65, "purple")]:
    assert render(input_dbz) == expected
print("ATILLA RainViewer palette and threshold checks: PASS")
