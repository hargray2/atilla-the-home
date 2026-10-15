#!/usr/bin/env python3
"""Check local Markdown links and image paths; no network access or writes."""
from __future__ import annotations

import re
import sys
from pathlib import Path
from urllib.parse import unquote, urlsplit

LINK = re.compile(r'!?\[[^\]\n]*\]\(([^)\n]+)\)')


def check_links(root: Path) -> list[str]:
    root = root.resolve()
    errors: list[str] = []
    for document in sorted(root.rglob("*.md")):
        if ".git" in document.relative_to(root).parts:
            continue
        for line_number, line in enumerate(document.read_text(encoding="utf-8").splitlines(), 1):
            for match in LINK.finditer(line):
                reference = match.group(1).strip().strip("<>")
                if not reference or reference.startswith("#"):
                    continue
                parsed = urlsplit(reference)
                if parsed.scheme or parsed.netloc or reference.startswith("//"):
                    continue
                local_path = unquote(parsed.path)
                if not local_path:
                    continue
                target = (document.parent / local_path).resolve()
                if not target.is_relative_to(root):
                    errors.append(f"{document.relative_to(root)}:{line_number}: link leaves repository: {reference}")
                elif not target.exists():
                    errors.append(f"{document.relative_to(root)}:{line_number}: missing: {reference}")
    return errors


def main() -> int:
    root = Path(sys.argv[1]) if len(sys.argv) > 1 else Path(__file__).resolve().parent.parent
    if not root.is_dir():
        print(f"ERROR: directory not found: {root}", file=sys.stderr)
        return 2
    issues = check_links(root)
    for issue in issues:
        print(issue)
    print(f"Markdown local-link audit: {'PASS' if not issues else 'FAIL'} ({len(issues)} issues)")
    return 1 if issues else 0


if __name__ == "__main__":
    raise SystemExit(main())
