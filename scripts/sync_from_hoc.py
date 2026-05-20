#!/usr/bin/env python3
"""
Smart HOC -> Ryazha-CLK Sync Script

Pulls commits from upstream Horizon-OC repo, adapts paths/names to our
Ryazha-CLK conventions, and creates clean commits without exposing HOC origin.

Features:
- Filename adaptation: hoc-clk -> ryazha-clk, hocclk -> rclk
- Content adaptation: HOC namespaces and symbols renamed
- Protected paths: skips Ryazha-Авто and VRR-related files
- Excludes: vrr, Horizon-OC-Monitor, deleted/renamed-by-us paths

Usage:
    python3 sync_from_hoc.py [--dry-run] [--since DAYS_BACK]
"""

import argparse
import os
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path
from typing import Iterable

# Ensure UTF-8 output even on Windows consoles
if hasattr(sys.stdout, "reconfigure"):
    try:
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
        sys.stderr.reconfigure(encoding="utf-8", errors="replace")
    except Exception:
        pass

UPSTREAM_REPO = "https://github.com/Horizon-OC/Horizon-OC.git"
UPSTREAM_BRANCH = "main"

# Path adaptation: HOC path -> our path
PATH_MAPPINGS = [
    ("Source/hoc-clk/", "Source/ryazha-clk/"),
    ("Source/Horizon-OC-Monitor/", "Source/Ryazha-Status-Monitor/"),
    ("dist/config/horizon-oc/", "dist/config/ryazha-clk/"),
    ("common/include/hocclk/", "common/include/rclk/"),
    ("common/include/hocclk.h", "common/include/rclk.h"),
]

# Content adaptation: token -> replacement
CONTENT_REPLACEMENTS = [
    # Commit-message-style prefix
    (r"\bhocclk:\s*", "ryazha-clk: "),
    (r"\bhoc-clk:\s*", "ryazha-clk: "),
    # Namespace/include renames
    (r"hocclk/", "rclk/"),
    (r"\"hocclk\.h\"", "\"rclk.h\""),
    (r"<hocclk\.h>", "<rclk.h>"),
    # Symbol renames
    (r"\bHOCCLK_", "RCLK_"),
    (r"\bHocClkGov\b", "RClkGov"),
    (r"\bHocClk\b", "RClk"),
    (r"\bHOCClk\b", "RCLK"),
    (r"\bHOC_", "RCLK_"),
    # Build artifact names
    (r"\bhoc-clk\.nsp\b", "ryazha-clk.nsp"),
    (r"\bhoc-clk\.ovl\b", "ryazha-clk.ovl"),
    (r"\bhoc\.kip\b", "rcu.kip"),
    # Config paths
    (r"\bhorizon-oc/\b", "ryazha-clk/"),
    # User-facing strings
    (r"\bHorizon-OC\b", "Ryazha-CLK"),
    (r"\bHorizon OC\b", "Ryazha CLK"),
    (r"\bHorizonOC\b", "RyazhaCLK"),
    (r"\bhorizon-oc\b", "ryazha-clk"),
    (r"\bhorizonoc\b", "ryazhaclk"),
]

# Paths we NEVER touch (these are ours, not theirs)
PROTECTED_PATHS = [
    "auto_ryazha",
    "Ryazha-Status-Monitor",
    "living_ladder",
    "display_hz_trackbar",
    "display_refresh_rate",
    ".github/workflows/",
    "scripts/sync_from_hoc.py",
    "ams_ver.txt",
    "README.md",
    "README_RU.md",
    "MIGRATION.md",
    ".gitmodules",
    ".gitignore",
]

# Paths to completely exclude from sync
EXCLUDED_PATHS = [
    "vrr/",
    "Source/Horizon-OC-Monitor/",
    "Source/vrr/",
    "build/",
    "dist/",
    "build__bak/",
    "fix_head.sh",
    "ams_patch.bat",
]


def run(cmd: list[str], cwd: str | None = None, check: bool = True) -> str:
    """Run a shell command and return stdout."""
    result = subprocess.run(
        cmd, cwd=cwd, check=check, capture_output=True, text=True, encoding="utf-8"
    )
    return result.stdout


def is_protected(path: str) -> bool:
    """Check if a path is protected (our code, don't overwrite)."""
    return any(p in path for p in PROTECTED_PATHS)


def is_excluded(path: str) -> bool:
    """Check if a path should be excluded from sync."""
    return any(path.startswith(p) or p in path for p in EXCLUDED_PATHS)


def adapt_path(path: str) -> str:
    """Transform HOC path to our path conventions."""
    for src, dst in PATH_MAPPINGS:
        if src in path:
            path = path.replace(src, dst)
    return path


def adapt_content(content: str) -> str:
    """Transform HOC content to our naming conventions."""
    for pattern, repl in CONTENT_REPLACEMENTS:
        content = re.sub(pattern, repl, content)
    return content


def clone_upstream(target_dir: Path) -> None:
    """Clone the upstream HOC repo."""
    print(f"[*] Cloning upstream {UPSTREAM_REPO} ...")
    run(["git", "clone", "--depth=50", "--branch", UPSTREAM_BRANCH,
         UPSTREAM_REPO, str(target_dir)])


def get_recent_commits(repo_dir: Path, since_days: int) -> list[str]:
    """Get commit SHAs from the last N days."""
    out = run(
        ["git", "log", f"--since={since_days} days ago", "--format=%H", "--reverse"],
        cwd=str(repo_dir),
    )
    return [line.strip() for line in out.splitlines() if line.strip()]


def get_changed_files(repo_dir: Path, commit: str) -> list[tuple[str, str]]:
    """Get files changed in a commit. Returns (status, path) tuples."""
    out = run(
        ["git", "show", "--name-status", "--format=", commit],
        cwd=str(repo_dir),
    )
    files = []
    for line in out.splitlines():
        line = line.strip()
        if not line:
            continue
        parts = line.split("\t")
        if len(parts) >= 2:
            files.append((parts[0], parts[-1]))
    return files


def get_file_content_at(repo_dir: Path, commit: str, path: str) -> str | None:
    """Get the content of a file at a specific commit."""
    try:
        return run(
            ["git", "show", f"{commit}:{path}"],
            cwd=str(repo_dir),
            check=True,
        )
    except subprocess.CalledProcessError:
        return None


def get_commit_message(repo_dir: Path, commit: str) -> tuple[str, str]:
    """Get commit subject and body."""
    subject = run(
        ["git", "log", "-1", "--format=%s", commit], cwd=str(repo_dir)
    ).strip()
    body = run(
        ["git", "log", "-1", "--format=%b", commit], cwd=str(repo_dir)
    ).strip()
    return subject, body


def commit_should_be_skipped(subject: str, body: str, files: list[tuple[str, str]]) -> str | None:
    """Decide if a commit should be skipped. Returns reason or None."""
    text = f"{subject}\n{body}".lower()

    # Skip merges and reverts of our changes
    if subject.lower().startswith("merge "):
        return "merge commit"

    # Skip commits that only touch excluded paths
    relevant = [p for _, p in files if not is_excluded(p)]
    if not relevant:
        return "only touches excluded paths"

    # Skip commits that conflict with our protected files
    if any(is_protected(p) for _, p in files):
        return "touches protected (Ryazha-Авто/VRR) files"

    # Skip commits with obvious topic conflicts
    if re.search(r"\bvrr\b|\bryazha\b|ryazha-авто|auto[\s_-]?ryazha", text):
        return "topic conflicts with our Ryazha/VRR features"

    return None


def apply_commit(our_repo: Path, upstream_repo: Path, commit: str, dry_run: bool) -> bool:
    """Apply a single HOC commit to our repo with path/content adaptation."""
    subject, body = get_commit_message(upstream_repo, commit)
    files = get_changed_files(upstream_repo, commit)

    skip_reason = commit_should_be_skipped(subject, body, files)
    if skip_reason:
        print(f"[-] Skip {commit[:7]}: {skip_reason}")
        return False

    print(f"[+] Apply {commit[:7]}: {subject}")
    if dry_run:
        for status, path in files:
            adapted = adapt_path(path)
            print(f"    {status} {path} -> {adapted}")
        return True

    # Apply each file change
    applied_any = False
    for status, path in files:
        if is_excluded(path) or is_protected(adapt_path(path)):
            continue
        our_path = our_repo / adapt_path(path)

        if status.startswith("D"):
            if our_path.exists():
                our_path.unlink()
                applied_any = True
            continue

        content = get_file_content_at(upstream_repo, commit, path)
        if content is None:
            continue

        # Only adapt content for text files
        if path.endswith((".cpp", ".hpp", ".h", ".c", ".md", ".json",
                          ".yml", ".sh", ".ini", "Makefile")):
            content = adapt_content(content)

        our_path.parent.mkdir(parents=True, exist_ok=True)
        our_path.write_text(content, encoding="utf-8")
        applied_any = True

    if not applied_any:
        return False

    # Make a clean commit without HOC attribution
    clean_subject = adapt_content(subject)
    run(["git", "add", "-A"], cwd=str(our_repo))

    # Check if anything actually changed
    diff = run(["git", "diff", "--cached", "--name-only"], cwd=str(our_repo))
    if not diff.strip():
        print(f"    (no net changes after adaptation)")
        return False

    run(["git", "commit", "-m", clean_subject], cwd=str(our_repo))
    return True


def main() -> int:
    parser = argparse.ArgumentParser(description="Sync from HOC with adaptation")
    parser.add_argument("--dry-run", action="store_true",
                        help="Show what would happen without changes")
    parser.add_argument("--since", type=int, default=21,
                        help="Days back to sync (default: 21)")
    parser.add_argument("--our-repo", default=os.getcwd(),
                        help="Path to our repo (default: cwd)")
    args = parser.parse_args()

    our_repo = Path(args.our_repo).resolve()
    if not (our_repo / ".git").exists():
        print(f"[!] {our_repo} is not a git repo")
        return 1

    with tempfile.TemporaryDirectory(prefix="hoc_sync_") as tmp:
        upstream = Path(tmp) / "upstream"
        clone_upstream(upstream)

        commits = get_recent_commits(upstream, args.since)
        print(f"[*] Found {len(commits)} commits in the last {args.since} days")

        applied = 0
        skipped = 0
        for c in commits:
            if apply_commit(our_repo, upstream, c, args.dry_run):
                applied += 1
            else:
                skipped += 1

        print()
        print(f"[*] Applied:  {applied}")
        print(f"[*] Skipped:  {skipped}")
        if args.dry_run:
            print("[*] Dry run - no actual changes were made")

    return 0


if __name__ == "__main__":
    sys.exit(main())
