import json
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# ---------------- Загрузка JSON ---------------- #
with open("results.json", "r") as f:
    data = json.load(f)

rows = []
for bench in data.get("benchmarks", []):
    name = bench["name"]  # например BM_Insert_Random/1024
    parts = name.split("/")
    base = parts[0]
    size = int(parts[1]) if len(parts) > 1 else None

    tokens = base.split("_")
    op = tokens[1] if len(tokens) > 1 else tokens[0]
    scenario = tokens[2] if len(tokens) > 2 else None

    time_raw = bench.get("real_time", bench.get("cpu_time", None))
    if time_raw is None or size is None:
        continue

    # Google Benchmark по умолчанию выводит время в наносекундах → переведём в миллисекунды
    time_ms = time_raw / 1e6

    rows.append({
        "operation": op,
        "scenario": scenario,
        "size": size,
        "time_ms": time_ms,
        "per_op_time": time_ms / size if size > 0 else np.nan
    })

df = pd.DataFrame(rows)

# ---------------- Функция для красивых графиков ---------------- #
def plot_lines(df_subset, title, filename, group_col):
    plt.figure(figsize=(10,6))
    for key, grp in df_subset.groupby(group_col):
        grp_sorted = grp.sort_values("size")
        plt.plot(grp_sorted["size"], grp_sorted["time_ms"], marker="o", label=str(key))

    plt.xlabel("Number of elements")
    plt.ylabel("Time (ms)")
    plt.title(title)
    plt.legend()
    plt.grid(True, which="both", linestyle="--", linewidth=0.5, alpha=0.7)
    plt.tight_layout()
    plt.savefig(filename, dpi=200)
    print(f"Saved {filename}")

# ---------------- Графики ---------------- #
# Insert: три сценария (Random, Ascending, Balanced)
df_insert = df[df["operation"] == "Insert"]
if not df_insert.empty:
    plot_lines(df_insert, "Insert performance (ms)", "insert_perf.png", "scenario")

# Find: только один сценарий (balanced init)
df_find = df[df["operation"] == "Find"]
if not df_find.empty:
    plot_lines(df_find, "Find performance (ms)", "find_perf.png", "operation")

# Erase: только один сценарий (balanced init)
df_erase = df[df["operation"] == "Erase"]
if not df_erase.empty:
    plot_lines(df_erase, "Erase performance (ms)", "erase_perf.png", "operation")
