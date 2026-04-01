#!/usr/bin/env python3
import argparse
import os
import sys

import pandas as pd
import matplotlib.pyplot as plt


def pick_col(df, candidates):
    for c in candidates:
        if c in df.columns:
            return c
    return None


def ensure_cols(df, required_any):
    """
    required_any: dict logical_name -> list of candidate column names
    """
    mapping = {}
    missing = []
    for logical, cands in required_any.items():
        col = pick_col(df, cands)
        if col is None:
            missing.append((logical, cands))
        else:
            mapping[logical] = col
    if missing:
        msg = ["CSV missing required columns:"]
        for logical, cands in missing:
            msg.append(f"- need '{logical}' column (one of: {', '.join(cands)})")
        msg.append(f"\nColumns present: {', '.join(df.columns)}")
        raise ValueError("\n".join(msg))
    return mapping


def save_fig(out_dir, name):
    png = os.path.join(out_dir, f"{name}.png")
    pdf = os.path.join(out_dir, f"{name}.pdf")
    plt.tight_layout()
    plt.savefig(png, dpi=200)
    plt.savefig(pdf)
    print(f"Saved: {png}")
    print(f"Saved: {pdf}")


def line_plot(df, x, y, series_col, title, xlabel, ylabel, out_dir, out_name):
    plt.figure()
    for key, g in df.groupby(series_col):
        g2 = g.sort_values(x)
        plt.plot(g2[x], g2[y], marker="o", label=str(key))
    plt.title(title)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.legend()
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)
    save_fig(out_dir, out_name)
    plt.close()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--csv", default="results.csv", help="Path to results.csv")
    ap.add_argument("--outdir", default="plots", help="Directory to write plots")
    ap.add_argument("--swap_for_size_plots", default="0,50",
                    help="Comma-separated swap values for total_vs_size plots, e.g. 0,50")
    ap.add_argument("--storage_for_swap_plots", type=int, default=10000,
                    help="Storage size for total_vs_swap and swaps_vs_swap plots")
    args = ap.parse_args()

    df = pd.read_csv(args.csv, sep=None, engine="python")  # auto-detect delimiter

    # Find columns (supports a few naming variants)
    cols = ensure_cols(df, {
        "impl": ["impl", "method", "lock", "sync"],
        "storage": ["storage", "storage_size", "size", "n", "N"],
        "swap": ["swap", "swap_chance", "swap_percent", "p_swap"],
        "total_per_sec": ["total_iters_per_sec", "total/s", "total_per_s", "total_per_sec"],
        "swaps_per_sec": ["swaps_per_sec", "swaps/s", "swaps_per_s"],
    })

    impl = cols["impl"]
    storage = cols["storage"]
    swap = cols["swap"]
    total_ps = cols["total_per_sec"]
    swaps_ps = cols["swaps_per_sec"]

    # Normalize numeric types
    df[storage] = pd.to_numeric(df[storage], errors="coerce")
    df[swap] = pd.to_numeric(df[swap], errors="coerce")
    df[total_ps] = pd.to_numeric(df[total_ps], errors="coerce")
    df[swaps_ps] = pd.to_numeric(df[swaps_ps], errors="coerce")
    df = df.dropna(subset=[impl, storage, swap, total_ps, swaps_ps])

    os.makedirs(args.outdir, exist_ok=True)

    # Plot 1: total_iters_per_sec vs storage for given swap levels (one plot per swap)
    swap_values = [int(s.strip()) for s in args.swap_for_size_plots.split(",") if s.strip()]
    for sv in swap_values:
        d = df[df[swap] == sv]
        if d.empty:
            print(f"Warn: no rows for swap={sv}")
            continue
        line_plot(
            d,
            x=storage,
            y=total_ps,
            series_col=impl,
            title=f"Пропускная способность читателей в зависимости от размера списка (swap = {sv}%)",
            xlabel="Размер списка (число элементов)",
            ylabel="Количество итераций в секунду",
            out_dir=args.outdir,
            out_name=f"total_vs_storage_swap{sv}"
        )


    # Plot 2: total_iters_per_sec vs swap for fixed storage
    d2 = df[df[storage] == args.storage_for_swap_plots]
    if d2.empty:
        print(f"Warn: no rows for storage={args.storage_for_swap_plots}")
    else:
        line_plot(
            d2,
            x=swap,
            y=total_ps,
            series_col=impl,
            title=f"Пропускная способность читателей в зависимости от вероятности swap (размер списка = {args.storage_for_swap_plots})",
            xlabel="Вероятность swap (%)",
            ylabel="Количество итераций в секунду",
            out_dir=args.outdir,
            out_name=f"total_vs_swap_storage{args.storage_for_swap_plots}"
        )



        # Plot 3: swaps_per_sec vs swap for fixed storage
        line_plot(
            d2,
            x=swap,
            y=swaps_ps,
            series_col=impl,
            title=f"Интенсивность операций swap в зависимости от вероятности swap (размер списка = {args.storage_for_swap_plots})",
            xlabel="Вероятность swap (%)",
            ylabel="Количество swap-операций в секунду",
            out_dir=args.outdir,
            out_name=f"swaps_vs_swap_storage{args.storage_for_swap_plots}"
        )


    # Extra: quick summary table CSV (nice for report)
    summary = (
        df.groupby([impl, storage, swap], as_index=False)[[total_ps, swaps_ps]]
          .mean()
          .sort_values([impl, storage, swap])
    )
    summary_path = os.path.join(args.outdir, "summary_mean.csv")
    summary.to_csv(summary_path, index=False)
    print(f"Saved: {summary_path}")


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(str(e), file=sys.stderr)
        sys.exit(1)
