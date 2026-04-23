import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("timings.csv")

df_120 = df[df["size"] == 120]
df_240 = df[df["size"] == 240]

fig, axes = plt.subplots(1, 2, figsize=(12, 5))

def plot_box(ax, data, title):
    grouped = data.groupby("P")["time"].apply(list)

    ax.boxplot(grouped, labels=grouped.index)
    ax.set_title(title)
    ax.set_xlabel("Processes (P)")
    ax.set_ylabel("Time (seconds)")
    ax.grid()

plot_box(axes[0], df_120, "Grid Size = 120³")
plot_box(axes[1], df_240, "Grid Size = 240³")

plt.tight_layout()
plt.savefig("final_plot.png")
plt.show()
