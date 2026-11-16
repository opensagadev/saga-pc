"""Start the Debug build under Wine and check that it reaches the title screen without a fault.

The Debug executable runs from the game data directory with `-NoFocus_Process` (it keeps running without
window focus) in build/smoke/, a directory of links to the game data, so its saves and settings stay
there and the data directory is never written. Its log (build/log/, one file per run) must show the title level loaded and a first frame
rendered, and no fault record (a first-chance fault diagnostic or an unhandled exception) may appear before
GRACE seconds after that frame. The run is stopped then, or after --timeout seconds without the frame.

The step needs the retail game data (GAME.DAT; SAGA_GAME_DATA or res/), a display and Wine; without one of
them, or under CI, it prints a skip line and passes. It opens a game window, so the proof runs it only when
asked (`tools/proof.py --smoke` or SAGA_SMOKE=1).

    python3 tools/smoke.py [--timeout 300] [--data DIR]
"""
import argparse
import contextlib
import os
import shutil
import signal
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
EXE = ROOT / "build/LEGOStarWarsSaga_debug.exe"
LOGS = ROOT / "build/log"
GRACE = 10
TITLE_LOADED = "World load complete"
TITLE_CONFIG = "config=levels\\titles\\titles"
FIRST_FRAME = "NuMain after render"
FAULTS = ("[FAULT-DIAGNOSTIC", "Unhandled exception")


def progress(text):
    """(title level loaded, first frame rendered, first fault line or None) from a Debug log."""
    title = TITLE_CONFIG in text and TITLE_LOADED in text
    frame = title and FIRST_FRAME in text[text.find(TITLE_CONFIG):]
    fault = next((line for line in text.splitlines() if line.startswith(FAULTS)), None)
    return title, frame, fault


def skip_reason(data, environ=os.environ):
    if environ.get("CI") or environ.get("GITHUB_ACTIONS"):
        return "running under CI"
    if data is None or not (data / "GAME.DAT").is_file():
        return "no game data (GAME.DAT in SAGA_GAME_DATA or res/)"
    if not environ.get("DISPLAY") and not environ.get("WAYLAND_DISPLAY"):
        return "no display"
    if not shutil.which("wine"):
        return "no wine"
    if not EXE.is_file():
        return f"{EXE.relative_to(ROOT)} is missing (make Debug)"
    return None


def game_data(argument):
    for candidate in (argument, os.environ.get("SAGA_GAME_DATA"), ROOT / "res"):
        if candidate and (Path(candidate) / "GAME.DAT").is_file():
            return Path(candidate).resolve()
    return None


def stage(data):
    """build/smoke: links to the data directory's entries; settings copied, saves and caches left out."""
    target = ROOT / "build/smoke"
    target.mkdir(parents=True, exist_ok=True)
    for entry in data.iterdir():
        if entry.name in ("SavedGames", "orig", "Movies") or entry.name.endswith(".dxvk-cache"):
            continue
        link = target / entry.name
        if entry.name.lower() == "pcconfig.txt":
            if not link.exists():
                shutil.copyfile(entry, link)
        elif not link.is_symlink():
            link.symlink_to(entry)
    return target


def new_log(before):
    logs = [p for p in LOGS.glob("*.log") if p not in before and not p.name.endswith(".fault.log")]
    return max(logs, key=lambda p: p.stat().st_mtime) if logs else None


def game_processes():
    """Unix pids whose program (argv[0]) is this tree's Debug executable or its capture helper. The game does
    not stay in the launcher's process group (Wine starts it through explorer), so it is found by name."""
    names = (str(EXE), str(EXE.with_name("saga_debug_capture.exe")))
    windows = tuple("Z:" + name.replace("/", "\\") for name in names)
    pids = []
    for entry in Path("/proc").iterdir():
        if not entry.name.isdigit() or int(entry.name) == os.getpid():
            continue
        try:
            program = (entry / "cmdline").read_bytes().split(b"\0")[0].decode("latin-1")
        except OSError:
            continue
        if program in names or program.startswith(windows) or program.lower() in (w.lower() for w in windows):
            pids.append(int(entry.name))
    return pids


def stop(game):
    for signal_number in (signal.SIGTERM, signal.SIGKILL):
        if game.poll() is None:
            with contextlib.suppress(ProcessLookupError):
                os.killpg(game.pid, signal_number)
        for pid in game_processes():
            with contextlib.suppress(ProcessLookupError):
                os.kill(pid, signal_number)
        deadline = time.monotonic() + 10
        while (game.poll() is None or game_processes()) and time.monotonic() < deadline:
            time.sleep(0.5)
        if game.poll() is not None and not game_processes():
            return


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--timeout", type=int, default=300, help="seconds to wait for the first title frame")
    parser.add_argument("--data", help="game data directory (default SAGA_GAME_DATA, then res/)")
    args = parser.parse_args(argv)
    data = game_data(args.data)
    reason = skip_reason(data)
    if not reason and game_processes():
        reason = "the Debug game of this tree is already running"
    if reason:
        print(f"smoke: skipped ({reason})")
        return 0
    before = set(LOGS.glob("*.log")) if LOGS.is_dir() else set()
    env = dict(os.environ, WINEPREFIX=str(ROOT / "build/tools/wine"), WINEDEBUG="-all",
               WINEDLLOVERRIDES="winemenubuilder.exe=d")
    start = time.monotonic()
    game = subprocess.Popen(["wine", "explorer", "/desktop=saga-smoke,1280x720", str(EXE), "-NoFocus_Process"], cwd=stage(data), env=env, stdin=subprocess.DEVNULL,
                            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, start_new_session=True)
    log, framed, verdict = None, None, None
    try:
        while verdict is None:
            time.sleep(1)
            log = log or new_log(before)
            title, frame, fault = progress(log.read_text(encoding="latin-1")) if log else (False, False, None)
            now = time.monotonic()
            if fault:
                verdict = f"FAIL; fault after {now - start:.0f} s: {fault.strip()[:160]}"
            elif frame and framed is None:
                framed = now
            elif framed is not None and now - framed >= GRACE:
                verdict = f"PASS; title screen after {framed - start:.0f} s, no fault {GRACE} s later"
            elif game.poll() is not None and framed is None:
                verdict = f"FAIL; the game exited ({game.returncode}) before the title screen"
            elif framed is None and now - start > args.timeout:
                verdict = f"FAIL; no title frame within {args.timeout} s" + ("" if title else " (title level not loaded)")
    finally:
        stop(game)
    print(f"smoke: {verdict}" + (f"; log {log.relative_to(ROOT)}" if log else ""))
    return 0 if verdict.startswith("PASS") else 1


if __name__ == "__main__":
    sys.exit(main())
