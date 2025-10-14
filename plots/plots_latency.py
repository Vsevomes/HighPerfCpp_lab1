import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from pathlib import Path
from matplotlib.backends.backend_pdf import PdfPages
import numpy as np

sns.set(style="whitegrid")
plt.rcParams["figure.figsize"] = (10, 6)

data_dir = Path("../plots")
pdf_output = Path("latency_distributions_report.pdf")

# Порог для усечения хвоста (в процентах)
cut_percentile = 99

csv_files = sorted(data_dir.glob("*.csv"))
if not csv_files:
    raise FileNotFoundError(f"Нет CSV-файлов в {data_dir}")

with PdfPages(pdf_output) as pdf:
    for file in csv_files:
        print(f"Processing: {file.name}")
        try:
            df = pd.read_csv(file)
        except Exception as e:
            print(f"⚠️ Ошибка чтения {file.name}: {e}")
            continue

        df["latency_us"] = df["latency_ns"] / 1000.0

        # Подвыборка, если слишком большой файл
        if len(df) > 200000:
            df = df.sample(200000, random_state=42)

        # --- Разбор имени файла ---
        parts = file.stem.split("_")
        op_type = parts[0] if len(parts) > 0 else "unknown"
        n_value = "?"

        if len(parts) == 4:
            scenario = parts[2]
            n_value = parts[3]
        elif len(parts) == 3:
            scenario = None
            n_value = parts[2]
        else:
            scenario = None

        # --- Заголовок ---
        title = f"{op_type.upper()} — N={n_value}"
        if scenario:
            title = f"{op_type.upper()} — {scenario}, N={n_value}"

        # --- Усечение хвоста по процентилю ---
        cutoff = np.percentile(df["latency_us"], cut_percentile)
        trimmed_df = df[df["latency_us"] <= cutoff]
        dropped_ratio = 1 - len(trimmed_df) / len(df)

        # --- Гистограмма ---
        plt.figure()
        sns.histplot(
            trimmed_df["latency_us"],
            bins=200,
            kde=False,
            color="steelblue",
            stat="probability"  # нормируем на вероятности
        )

        # Добавим вертикальную линию — граница p99
        plt.axvline(cutoff, color="red", linestyle="--", label=f"{cut_percentile}th percentile")

        # --- Подписи и оформление ---
        plt.title(f"{title}\n(Обрезано {dropped_ratio:.2%} верхнего хвоста)")
        plt.xlabel("Задержка (мкс)")
        plt.ylabel("Вероятность")
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        pdf.savefig()
        plt.close()

print(f"\n✅ Отчёт успешно сохранён: {pdf_output.resolve()}")
