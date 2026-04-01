#!/usr/bin/env python3
"""Benchmark runner for lab 2.3 (linked list synchronization variants).

Compiles and runs:
  - list-mutex.c    -> ./list_mutex
  - list-spinlock.c -> ./list_spin
  - list-rwlock.c   -> ./list_rw

Programs are expected to accept:
  <storage_size> <swap_chance 0..100> [seconds=5] [quiet=0|1]

and print a single summary line:
  RESULT storage=... swap=... sec=... inc=... dec=... eq=... swap_iters=... swaps=...

This script runs a grid over queue sizes and swap chances and writes CSV.
"""

from __future__ import annotations

import argparse
import csv
import itertools
import re
import subprocess
from pathlib import Path

BINS = {
    "mutex":  ("list-mutex.c", "list_mutex"),
    "spin":   ("list-spinlock.c", "list_spin"),
    "rwlock": ("list-rwlock.c", "list_rw"),
}

RESULT_RE = re.compile(
    r"RESULT\s+storage=(\d+)\s+swap=(\d+)\s+sec=(\d+)\s+inc=(\d+)\s+dec=(\d+)\s+eq=(\d+)\s+swap_iters=(\d+)\s+swaps=(\d+)"
)


def sh(cmd: list[str]) -> str:
    return subprocess.check_output(cmd, text=True)


def compile_all() -> None:
    for name, (src, out) in BINS.items():
        cmd = [
            "gcc",
            "-O2",
            "-pthread",
            "-std=c11",
            "-Wall",
            "-Wextra",
            src,
            "-o",
            out,
        ]
        subprocess.check_call(cmd)


def run_one(bin_path: str, storage: int, swap: int, seconds: int) -> dict:
    # quiet=1 to avoid per-thread spam
    proc = subprocess.run(
        [bin_path, str(storage), str(swap), str(seconds), "1"],
        text=True,
        capture_output=True,
        check=True,
    )
    out = (proc.stdout + "\n" + proc.stderr).strip()
    m = None
    for line in out.splitlines()[::-1]:
        m = RESULT_RE.search(line)
        if m:
            break
    if not m:
        raise RuntimeError(f"No RESULT line found. Output was:\n{out}")

    storage_i, swap_i, sec_i, inc_i, dec_i, eq_i, swap_iters_i, swaps_i = map(int, m.groups())

    # Throughput metrics (per second)
    total_iters = inc_i + dec_i + eq_i + swap_iters_i
    return {
        "storage": storage_i,
        "swap_chance": swap_i,
        "seconds": sec_i,
        "inc_iters": inc_i,
        "dec_iters": dec_i,
        "eq_iters": eq_i,
        "swap_iters": swap_iters_i,
        "swaps": swaps_i,
        "total_iters": total_iters,
        "total_iters_per_sec": total_iters / sec_i if sec_i else 0.0,
        "swaps_per_sec": swaps_i / sec_i if sec_i else 0.0,
    }


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--seconds", type=int, default=5)
    ap.add_argument("--out", type=str, default="results.csv")
    ap.add_argument(
        "--sizes",
        type=str,
        default="1000,10000,100000",
        help="comma-separated storage sizes",
    )
    ap.add_argument(
        "--swaps",
        type=str,
        default="0,1,10,50,90",
        help="comma-separated swap chances (0..100)",
    )
    args = ap.parse_args()

    sizes = [int(x) for x in args.sizes.split(",") if x.strip()]
    swaps = [int(x) for x in args.swaps.split(",") if x.strip()]

    compile_all()

    rows: list[dict] = []
    for impl, (_, exe) in BINS.items():
        bin_path = str(Path(".") / exe)
        if not bin_path.startswith("./"):
            bin_path = "./" + bin_path
        for storage, swap in itertools.product(sizes, swaps):
            r = run_one(bin_path, storage, swap, args.seconds)
            r["impl"] = impl
            rows.append(r)
            print(
                f"{impl:6s} storage={storage:<7d} swap={swap:<3d} total/s={r['total_iters_per_sec']:.2f} swaps/s={r['swaps_per_sec']:.2f}"
            )

    # Sort for nicer CSV
    rows.sort(key=lambda d: (d["storage"], d["swap_chance"], d["impl"]))

    out_path = Path(args.out)
    with out_path.open("w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=list(rows[0].keys()))
        w.writeheader()
        w.writerows(rows)

    print(f"\nWrote {out_path.resolve()}")


if __name__ == "__main__":
    main()
