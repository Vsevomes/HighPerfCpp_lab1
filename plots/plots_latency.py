import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from pathlib import Path
from matplotlib.backends.backend_pdf import PdfPages

sns.set(style="whitegrid")
plt.rcParams["figure.figsize"] = (10, 6)

data_dir = Path("../plots")
pdf_output = Path("latency_distributions_report.pdf")

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

        # Преобразуем наносекунды в микросекунды
        df["latency_us"] = df["latency_ns"] / 1000.0

        # Подвыборка, если слишком большой файл
        if len(df) > 200000:
            df = df.sample(200000, random_state=42)

        # --- Разбор имени файла ---
        # Возможные варианты:
        # 1. insert_latencies_random_4096.csv
        # 2. find_latencies_8192.csv
        # 3. erase_latencies_16384.csv
        parts = file.stem.split("_")
        op_type = parts[0] if len(parts) > 0 else "unknown"
        n_value = "?"

        if len(parts) == 4:
            # insert_latencies_random_4096
            scenario = parts[2]
            n_value = parts[3]
        elif len(parts) == 3:
            # find_latencies_8192
            scenario = None
            n_value = parts[2]
        else:
            scenario = None

        # --- Заголовок ---
        title = f"{op_type.upper()} — N={n_value}"
        if scenario:
            title = f"{op_type.upper()} — {scenario}, N={n_value}"

        # --- Гистограмма ---
        plt.figure()
        sns.histplot(df["latency_us"], bins=200, kde=False, color="steelblue")
        plt.title(title)
        plt.xlabel("Латентность (мкс)")
        plt.ylabel("Частота")
        plt.grid(True, alpha=0.3)
        pdf.savefig()
        plt.close()

        # --- Boxplot ---
        plt.figure()
        sns.boxplot(x=df["latency_us"], color="lightcoral")
        plt.title(f"Boxplot: {title}")
        plt.xlabel("Латентность (мкс)")
        plt.grid(True, alpha=0.3)
        pdf.savefig()
        plt.close()

print(f"\n✅ Отчёт успешно сохранён: {pdf_output.resolve()}")
