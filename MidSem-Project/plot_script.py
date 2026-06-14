import pandas as pd
import matplotlib.pyplot as pd_plt
import seaborn as sns

df = pd.read_csv("timing_data.txt")

pd_plt.figure(figsize=(10, 6))

sns.boxplot(x="P", y="Time", hue="M", data=df, palette="Set2")

pd_plt.title("Execution Time Distribution by Process Count and Data Size")
pd_plt.xlabel("Number of Processes (P)")
pd_plt.ylabel("Time (seconds)")
pd_plt.grid(True, linestyle='--', alpha=0.7)

#save
pd_plt.savefig("time_vs_processes_boxplot.png", dpi=300, bbox_inches='tight')

print("Plot saved as time_vs_processes_boxplot.png")
