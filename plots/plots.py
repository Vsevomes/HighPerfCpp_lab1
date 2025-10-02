import json
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

with open("results.json", "r") as f:
    data = json.load(f)

rows = []
for bench in data.get("benchmarks", []):
    name = bench["name"] 
    parts = name.split("/")
    base = parts[0]
    size = int(parts[1]) if len(parts) > 1 else None

    tokens = base.split("_")

    if base == "BM_Insert":
        op = "InsertSingle"
        scenario = None
    else:
        op = tokens[1] if len(tokens) > 1 else tokens[0]
        scenario = tokens[2] if len(tokens) > 2 else None

    time_raw = bench.get("real_time", bench.get("cpu_time", None))
    if time_raw is None or size is None:
        continue

    rows.append({
        "operation": op,
        "scenario": scenario,
        "size": size,
        "time_ns": time_raw ,
        "per_op_time": time_raw  / size if size > 0 else np.nan
    })

df = pd.DataFrame(rows)

def plot_lines(df_subset, title, filename, group_col):
    plt.figure(figsize=(10,6))
    for key, grp in df_subset.groupby(group_col):
        grp_sorted = grp.sort_values("size")
        plt.plot(grp_sorted["size"], grp_sorted["time_ns"], marker="o", label=str(key))

    plt.xlabel("Number of elements")
    plt.ylabel("Time (ns)")
    plt.title(title)
    plt.legend()
    plt.grid(True, which="both", linestyle="--", linewidth=0.5, alpha=0.7)
    plt.tight_layout()
    plt.savefig(filename, dpi=200)
    print(f"Saved {filename}")

# ---------------- Графики ---------------- #
# Insert: three scenario (Random, Ascending, Balanced)
df_insert = df[df["operation"] == "Insert"]
if not df_insert.empty:
    plot_lines(df_insert, "Insert performance (ns)", "insert_perf.png", "scenario")

# Single Insert
df_insert_single = df[df["operation"] == "InsertSingle"]
if not df_insert_single.empty:
    plot_lines(df_insert_single, "Single Insert performance (ns)", "insert_single_perf.png", "operation")

# Find
df_find = df[df["operation"] == "Find"]
if not df_find.empty:
    plot_lines(df_find, "Find performance (ns)", "find_perf.png", "operation")

# Erase
df_erase = df[df["operation"] == "Erase"]
if not df_erase.empty:
    plot_lines(df_erase, "Erase performance (ns)", "erase_perf.png", "operation")
