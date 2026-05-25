#!/usr/bin/env python3
"""
sync_from_hoc.py -- ADVISORY upstream HOC -> Ryazha-CLK diff inspector.

Что делает по умолчанию:
  python3 scripts/sync_from_hoc.py
  - клонирует upstream (Horizon-OC/Horizon-OC),
  - проходит по последним коммитам,
  - выводит, какие файлы upstream трогает и что бы с ними произошло
    после нашей path/content adaptation (PROTECT / EXCLUDE / WRITE / LAYOUT-CONFLICT),
  - НИЧЕГО НЕ ПИШЕТ И НЕ КОММИТИТ.

Чтобы реально применить -- нужен явный --apply.
Даже с --apply скрипт:
  - не делает git commit (только пишет файлы; staging + commit -- руками,
    чтобы код-ревью был обязателен);
  - пропускает любой upstream-файл, для которого в нашем repo уже есть
    эквивалент в subdir-layout (см. detect_layout_conflict);
  - пропускает любой path в PROTECTED_PATHS (наши собственные файлы);
  - пропускает любой path в EXCLUDED_PATHS (фичи, которые мы переписали).

Почему "advisory by default":
RCU и HOC расходятся структурно (flat src/ -> subdir layout, nxExt
extraction, RClk* rename, auto_ryazha, libryazhahand submodule, PNG
wallpaper и т.д.). Любая попытка autosync затирала наши изменения и
ломала сборку. Скрипт стал инструментом ревью, а не автомата.

Раньше существовал .github/workflows/sync_from_hoc.yml с cron'ом 04:00 UTC --
он был удалён по этой же причине. Запускайте вручную с явным контролем.
"""

from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
import tempfile
from pathlib import Path

# Ensure UTF-8 console output even on Windows.
if hasattr(sys.stdout, "reconfigure"):
    try:
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
        sys.stderr.reconfigure(encoding="utf-8", errors="replace")
    except Exception:
        pass


UPSTREAM_REPO = "https://github.com/Horizon-OC/Horizon-OC.git"
UPSTREAM_BRANCH = "main"


# ──────────────────────────────────────────────────────────────────────
# Path adaptation: upstream-путь -> наш путь.
# Только префиксные/директорные ренеймы. Внутренний переезд upstream-flat
# (Source/hoc-clk/sysmodule/src/<file>.cpp) в наш subdir-layout
# (Source/ryazha-clk/sysmodule/src/<group>/<file>.cpp) сюда НЕ кладём --
# это решает detect_layout_conflict() runtime'ом, сравнивая с тем, что
# реально лежит в репо.
# ──────────────────────────────────────────────────────────────────────
PATH_MAPPINGS: list[tuple[str, str]] = [
    ("Source/hoc-clk/",            "Source/ryazha-clk/"),
    ("Source/Horizon-OC-Monitor/", "Source/Ryazha-Status-Monitor/"),
    ("dist/config/horizon-oc/",    "dist/config/ryazha-clk/"),
    ("common/include/hocclk/",     "common/include/rclk/"),
    ("common/include/hocclk.h",    "common/include/rclk.h"),
    ("hocclk/",                    "rclk/"),
]


# ──────────────────────────────────────────────────────────────────────
# Content adaptation: token -> replacement (regex).
# Длинные паттерны перед короткими, чтобы Horizon-OC не битался от horizon-oc.
# Применяется ТОЛЬКО к текстовым файлам (см. is_text_path).
# ──────────────────────────────────────────────────────────────────────
CONTENT_REPLACEMENTS: list[tuple[str, str]] = [
    # commit-msg-style prefix
    (r"\bhocclk:\s*",  "ryazha-clk: "),
    (r"\bhoc-clk:\s*", "ryazha-clk: "),

    # namespaces / includes
    (r"hocclk/",         "rclk/"),
    (r"\"hocclk\.h\"",   "\"rclk.h\""),
    (r"<hocclk\.h>",     "<rclk.h>"),
    (r"\"hocclk\.hpp\"", "\"rclk.hpp\""),
    (r"<hocclk\.hpp>",   "<rclk.hpp>"),

    # symbol renames (длинные раньше коротких)
    (r"\bHOCCLK_",                 "RCLK_"),
    (r"\bHocClkGov\b",             "RClkGov"),
    (r"\bHocClk\b",                "RClk"),
    (r"\bHOCClk\b",                "RCLK"),
    (r"\bHOC_",                    "RCLK_"),
    (r"\bhocclkGetVersionString\b","rclkGetVersionString"),
    (r"\bhocclkGetConfigValues\b", "rclkGetConfigValues"),
    (r"\bhocclkSetConfigValues\b", "rclkSetConfigValues"),
    (r"\bhocclk([A-Z]\w*)\b",      r"rclk\1"),

    # artifacts
    (r"\bhoc-clk\.nsp\b", "ryazha-clk.nsp"),
    (r"\bhoc-clk\.ovl\b", "ryazha-clk.ovl"),
    (r"\bhoc-clk\.kip\b", "rcu.kip"),
    (r"\bhoc\.kip\b",     "rcu.kip"),
    (r"\bhoc-clk\.nro\b", "ryazha-clk.nro"),
    (r"\bhoc-clk\.elf\b", "ryazha-clk.elf"),

    # config paths
    (r"\bhorizon-oc/\b",    "ryazha-clk/"),
    (r"/config/horizon-oc", "/config/ryazha-clk"),
    (r"\[horizon-oc\]",     "[ryazha-clk]"),
    (r"\[hoc-clk\]",        "[ryazha-clk]"),

    # repo URLs
    (r"github\.com/Horizon-OC/Horizon-OC",
     "github.com/Dimasick-git/RCU"),
    (r"github\.com/Horizon-OC/",
     "github.com/Dimasick-git/"),

    # user-facing brand strings
    (r"\bHorizon-OC\b", "Ryazha-CLK"),
    (r"\bHorizon OC\b", "Ryazha CLK"),
    (r"\bHorizonOC\b",  "RyazhaCLK"),
    (r"\bhorizon-oc\b", "ryazha-clk"),
    (r"\bhorizonoc\b",  "ryazhaclk"),
    (r"\bHOC\b",        "RCLK"),
    (r"\bhoc\b",        "ryazha-clk"),
]


# ──────────────────────────────────────────────────────────────────────
# PROTECTED_PATHS -- наш код или наша конфигурация. Никогда не overwrite-аем.
# Pattern с '/' в конце = directory prefix. Иначе full path или basename match
# (см. _matches_pattern).
# ──────────────────────────────────────────────────────────────────────
PROTECTED_PATHS: list[str] = [
    # ── уникальные фичи ──
    "auto_ryazha",
    "Ryazha-Status-Monitor",
    "living_ladder",
    "display_hz_trackbar",
    "display_refresh_rate",

    # ── весь sysmodule (мы перенесли его в subdir layout, любой
    #     upstream flat-write попадёт сюда и будет conflict'ом) ──
    "Source/ryazha-clk/sysmodule/src/",
    "Source/ryazha-clk/sysmodule/Makefile",
    "Source/ryazha-clk/sysmodule/toolbox.json",
    "Source/ryazha-clk/sysmodule/perms.json",

    # ── весь overlay + i18n + кастомный UI ──
    "Source/ryazha-clk/overlay/src/",
    "Source/ryazha-clk/overlay/lang/",
    "Source/ryazha-clk/overlay/Makefile",
    "Source/ryazha-clk/overlay/lib/libryazhahand",

    # ── общий код (включая client/ipc.* с RClk*-renames и
    #     auto_ryazha.h, который мы добавили) ──
    "Source/ryazha-clk/common/include/rclk/auto_ryazha.h",
    "Source/ryazha-clk/common/include/rclk/client/ipc.h",
    "Source/ryazha-clk/common/include/rclk/ipc.h",
    "Source/ryazha-clk/common/include/rclk/config.h",
    "Source/ryazha-clk/common/include/i2c.h",
    "Source/ryazha-clk/common/src/client/ipc.c",

    # ── сборочная инфраструктура ──
    "Source/ryazha-clk/build.sh",
    "Source/ryazha-clk/config.ini.template",
    "Source/ryazha-clk/MIGRATION.md",
    "Source/ryazha-clk/LICENSE",
    "Source/ryazha-clk/README.md",
    "Source/ryazha-clk/bitmap.py",

    # ── репо в целом ──
    ".github/",
    "scripts/",
    "build.sh",
    "collect_dist.sh",
    "ams_ver.txt",
    "ams_patch.bat",
    "fix_head.sh",
    "README.md",
    "README_RU.md",
    "MIGRATION.md",
    "COMPILING.md",
    "CONTRIBUTING.md",
    "SECURITY.md",
    "CODE_OF_CONDUCT.md",
    "LICENSE",
    ".gitmodules",
    ".gitignore",
    ".gitattributes",
]


# ──────────────────────────────────────────────────────────────────────
# EXCLUDED_PATHS -- HOC-фичи, которые мы переписали или вытащили
# в submodule. Их и в дайджест не показываем.
# ──────────────────────────────────────────────────────────────────────
EXCLUDED_PATHS: list[str] = [
    "vrr/",
    "Source/Horizon-OC-Monitor/",
    "Source/vrr/",
    "Source/Atmosphere/",
    "Source/Atmosphere-Patches/",
    "Source/Old-Atmosphere-Patches/",
    "Source/SaltyNX/",
    "Source/MemTesterNX/",
    "Source/TinyMemBenchNX/",
    "Source/fatal_handler_payload/",
    "build/",
    "build__bak/",
    "dist/",
    "Source/ryazha-clk/dist/",
    "Source/ryazha-clk/overlay/build/",
    "Source/ryazha-clk/overlay/out/",
    "Source/ryazha-clk/sysmodule/build/",
    "Source/ryazha-clk/sysmodule/out/",
    "fix_head.sh",
    "ams_patch.bat",
]


# ──────────────────────────────────────────────────────────────────────
# Расширения / имена текстовых файлов. Adaptation применяется только к ним.
# ──────────────────────────────────────────────────────────────────────
TEXT_EXTENSIONS: tuple[str, ...] = (
    ".cpp", ".hpp", ".h", ".c", ".cc", ".cxx", ".inc",
    ".md", ".txt", ".json", ".yml", ".yaml",
    ".sh", ".bat", ".ps1",
    ".ini", ".cfg", ".conf", ".toml",
    ".py", ".rb", ".lua",
    ".mk", ".cmake",
    ".gitignore", ".gitattributes", ".gitmodules",
    ".clang-format", ".clang-tidy", ".editorconfig",
    ".html", ".css", ".js",
)

TEXT_BASENAMES: tuple[str, ...] = (
    "Makefile", "LICENSE", "AUTHORS", "CHANGELOG", "README",
    "Dockerfile", "Doxyfile",
)


# ──────────────────────────────────────────────────────────────────────
# Subprocess helpers.
# ──────────────────────────────────────────────────────────────────────
def run(cmd: list[str], cwd: str | None = None, check: bool = True) -> str:
    r = subprocess.run(cmd, cwd=cwd, check=check, capture_output=True)
    return r.stdout.decode("utf-8", errors="replace")


def run_bytes(cmd: list[str], cwd: str | None = None, check: bool = True) -> bytes:
    r = subprocess.run(cmd, cwd=cwd, check=check, capture_output=True)
    return r.stdout


# ──────────────────────────────────────────────────────────────────────
# Matching.
# ──────────────────────────────────────────────────────────────────────
def _matches_pattern(path: str, pattern: str) -> bool:
    if pattern.endswith("/"):
        return path.startswith(pattern) or ("/" + pattern) in ("/" + path)
    if path == pattern:
        return True
    if path.startswith(pattern + "/"):
        return True
    if "/" not in pattern and os.path.basename(path) == pattern:
        return True
    return False


def is_protected(path: str) -> bool:
    candidates = (path, adapt_path(path))
    return any(_matches_pattern(p, pat) for p in candidates for pat in PROTECTED_PATHS)


def is_excluded(path: str) -> bool:
    candidates = (path, adapt_path(path))
    return any(_matches_pattern(p, pat) for p in candidates for pat in EXCLUDED_PATHS)


def adapt_path(path: str) -> str:
    for src, dst in PATH_MAPPINGS:
        if src in path:
            path = path.replace(src, dst)
    return path


def adapt_content(content: str) -> str:
    for pattern, repl in CONTENT_REPLACEMENTS:
        content = re.sub(pattern, repl, content)
    return content


def is_text_path(path: str) -> bool:
    lower = path.lower()
    if lower.endswith(TEXT_EXTENSIONS):
        return True
    basename = os.path.basename(path)
    return basename in TEXT_BASENAMES or any(basename.startswith(b) for b in TEXT_BASENAMES)


# ──────────────────────────────────────────────────────────────────────
# Layout conflict detector.
# Upstream HOC хранит код в flat src/, мы -- в src/<group>/. Если у нас
# уже есть файл с тем же базовым именем в одном из known subdir'ов,
# upstream'овский flat-вариант -- это конфликт layout'а; пишут его
# только вручную после ревью.
# ──────────────────────────────────────────────────────────────────────
SYSMODULE_SUBDIRS: tuple[str, ...] = (
    "src/board", "src/display", "src/file", "src/hos", "src/i2c",
    "src/ipc", "src/mapping", "src/mgr", "src/pwr", "src/soc",
    "src/tsensor", "src/util",
)


def detect_layout_conflict(our_repo: Path, adapted_path: str) -> str | None:
    """Если adapted_path -- это flat-файл в Source/ryazha-clk/sysmodule/src/
    или /overlay/src/, и для него есть subdir-эквивалент в нашем repo --
    верни путь конфликтующего файла. Иначе None."""
    p = Path(adapted_path)
    parts = p.parts
    # ловим только sysmodule flat src/ файлы. Overlay тоже плоский, но
    # внутри src/ui/gui/* etc. -- у нас upstream туда же кладёт.
    try:
        idx = parts.index("sysmodule")
    except ValueError:
        return None
    if idx + 2 >= len(parts):
        return None
    if parts[idx + 1] != "src":
        return None
    if (idx + 3) != len(parts):
        # flat = ровно sysmodule/src/<file>. Если внутри уже subdir --
        # upstream сам кладёт в subdir, конфликта по layout'у нет.
        return None
    filename = parts[-1]
    sysmodule_root = Path(*parts[:idx + 1])
    for sub in SYSMODULE_SUBDIRS:
        candidate = our_repo / sysmodule_root / sub / filename
        if candidate.exists():
            return str(candidate.relative_to(our_repo))
    return None


# ──────────────────────────────────────────────────────────────────────
# Git helpers против upstream-клона.
# ──────────────────────────────────────────────────────────────────────
def clone_upstream(target: Path) -> None:
    print(f"[*] cloning {UPSTREAM_REPO}")
    run(["git", "clone", "--depth=50", "--branch", UPSTREAM_BRANCH, UPSTREAM_REPO, str(target)])


def get_recent_commits(repo: Path, since_days: int) -> list[str]:
    out = run(
        ["git", "log", f"--since={since_days} days ago", "--format=%H", "--reverse"],
        cwd=str(repo),
    )
    return [l.strip() for l in out.splitlines() if l.strip()]


def get_changed_files(repo: Path, commit: str) -> list[tuple[str, str]]:
    out = run(["git", "show", "--name-status", "--format=", commit], cwd=str(repo))
    files: list[tuple[str, str]] = []
    for line in out.splitlines():
        line = line.strip()
        if not line:
            continue
        parts = line.split("\t")
        if len(parts) >= 2:
            files.append((parts[0], parts[-1]))
    return files


def get_file_content_at(repo: Path, commit: str, path: str) -> bytes | None:
    try:
        return run_bytes(["git", "show", f"{commit}:{path}"], cwd=str(repo), check=True)
    except subprocess.CalledProcessError:
        return None


def get_commit_message(repo: Path, commit: str) -> tuple[str, str]:
    subject = run(["git", "log", "-1", "--format=%s", commit], cwd=str(repo)).strip()
    body    = run(["git", "log", "-1", "--format=%b", commit], cwd=str(repo)).strip()
    return subject, body


# ──────────────────────────────────────────────────────────────────────
# Decide per-file what would happen.
# ──────────────────────────────────────────────────────────────────────
def classify(our_repo: Path, path: str) -> tuple[str, str]:
    """Returns (verdict, detail). Verdict is one of:
      EXCLUDE  -- из EXCLUDED_PATHS, не показываем даже в advisory.
      PROTECT  -- из PROTECTED_PATHS, наш код, никогда не overwrite.
      LAYOUT   -- upstream flat-файл, у нас subdir-эквивалент => конфликт.
      WRITE    -- безопасно записать (но всё равно требует --apply).
    """
    if is_excluded(path):
        return ("EXCLUDE", "excluded path")
    if is_protected(path):
        return ("PROTECT", "in PROTECTED_PATHS")
    adapted = adapt_path(path)
    conflict = detect_layout_conflict(our_repo, adapted)
    if conflict:
        return ("LAYOUT", f"upstream flat conflict with {conflict}")
    return ("WRITE", adapted)


# ──────────────────────────────────────────────────────────────────────
# Commit-level skip heuristic.
# ──────────────────────────────────────────────────────────────────────
def should_skip_commit(subject: str, body: str, files: list[tuple[str, str]], our_repo: Path) -> str | None:
    text = f"{subject}\n{body}".lower()

    if subject.lower().startswith("merge "):
        return "merge commit"

    actionable = []
    for _, p in files:
        verdict, _ = classify(our_repo, p)
        if verdict == "WRITE":
            actionable.append(p)
    if not actionable:
        return "no writable files (everything excluded/protected/layout-conflict)"

    if re.search(r"\bvrr\b|\bryazha\b|ryazha-авто|auto[\s_-]?ryazha", text):
        return "topic conflicts with our Ryazha/VRR features"

    return None


# ──────────────────────────────────────────────────────────────────────
# Apply (only if --apply).
# ──────────────────────────────────────────────────────────────────────
def apply_file(our_repo: Path, upstream_repo: Path, commit: str, status: str, path: str) -> bool:
    adapted = adapt_path(path)
    our_path = our_repo / adapted

    if status.startswith("D"):
        if our_path.exists():
            our_path.unlink()
            print(f"    del  {adapted}")
            return True
        return False

    raw = get_file_content_at(upstream_repo, commit, path)
    if raw is None:
        return False

    our_path.parent.mkdir(parents=True, exist_ok=True)

    if is_text_path(path):
        try:
            text = raw.decode("utf-8")
        except UnicodeDecodeError:
            print(f"    skip {path}: non-UTF8 text -- saved as bytes")
            our_path.write_bytes(raw)
        else:
            text = adapt_content(text)
            our_path.write_text(text, encoding="utf-8")
    else:
        our_path.write_bytes(raw)

    print(f"    {status:<3} {adapted}")
    return True


def process_commit(our_repo: Path, upstream_repo: Path, commit: str, apply: bool) -> tuple[int, int]:
    """Returns (would_write_count, skipped_count)."""
    subject, body = get_commit_message(upstream_repo, commit)
    files = get_changed_files(upstream_repo, commit)

    reason = should_skip_commit(subject, body, files, our_repo)
    if reason:
        print(f"[-] skip {commit[:7]}: {reason}  ({subject})")
        return 0, 1

    print(f"[+] {commit[:7]}: {subject}")
    writes = 0
    for status, path in files:
        verdict, detail = classify(our_repo, path)
        if verdict == "EXCLUDE":
            continue  # тихо
        if verdict == "PROTECT":
            print(f"    PROTECT  {path}  ({detail})")
            continue
        if verdict == "LAYOUT":
            print(f"    LAYOUT!  {path}  ({detail})")
            continue
        # WRITE
        if apply:
            if apply_file(our_repo, upstream_repo, commit, status, path):
                writes += 1
        else:
            print(f"    WRITE    {path} -> {detail}")
            writes += 1
    return writes, 0


# ──────────────────────────────────────────────────────────────────────
# main
# ──────────────────────────────────────────────────────────────────────
def main() -> int:
    parser = argparse.ArgumentParser(
        description="Advisory upstream sync for Ryazha-CLK. Default is dry-run."
    )
    parser.add_argument("--apply", action="store_true",
                        help="Actually write files (без commit'а). По умолчанию -- только показывает.")
    parser.add_argument("--since", type=int, default=21,
                        help="Days back to inspect (default: 21)")
    parser.add_argument("--our-repo", default=os.getcwd(),
                        help="Path to our repo (default: cwd)")
    args = parser.parse_args()

    our_repo = Path(args.our_repo).resolve()
    if not (our_repo / ".git").exists():
        print(f"[!] {our_repo} is not a git repo")
        return 1

    if not args.apply:
        print("=" * 64)
        print("ADVISORY MODE -- ничего не пишется. Запусти с --apply, если уверен.")
        print("Даже с --apply: НИ ЕДИНОГО git commit не делается -- ревью обязателен.")
        print("=" * 64)

    with tempfile.TemporaryDirectory(prefix="rcu_sync_") as tmp:
        upstream = Path(tmp) / "upstream"
        clone_upstream(upstream)

        commits = get_recent_commits(upstream, args.since)
        print(f"[*] {len(commits)} commits за последние {args.since} дней")

        total_writes = 0
        total_skips = 0
        for c in commits:
            w, s = process_commit(our_repo, upstream, c, apply=args.apply)
            total_writes += w
            total_skips += s

        print()
        if args.apply:
            print(f"[*] written:  {total_writes} files")
            print(f"[*] commits skipped: {total_skips}")
            print()
            print("Next steps (manual):")
            print("  git status                 # ревью изменений")
            print("  git diff                   # построчно")
            print("  git add <files>            # стейджим только то, что прошло ревью")
            print("  git commit -m 'sync: ...'  # коммитим САМИ")
        else:
            print(f"[*] would write: {total_writes} files")
            print(f"[*] commits skipped: {total_skips}")
            print("Запусти с --apply, чтобы реально записать (но коммит -- руками).")

    return 0


if __name__ == "__main__":
    sys.exit(main())
