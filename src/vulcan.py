import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

plt.style.use('miller.mplstyle')

au = 1.496e11

def plot_positions(_x, _y, label):
    x = np.array(_x) / au
    y = np.array(_y) / au

    xlim = list(plt.xlim())
    ylim = list(plt.ylim())

    xr = max(np.abs(xlim + [x[0] * 1.05]))
    yr = max(np.abs(ylim + [y[0] * 1.05]))
    r = max([xr, yr])

    plt.plot(x[0], y[0], '.', label=label, markersize=10, zorder=1)
    plt.plot(x, y, zorder=0, linewidth=1.5)

    plt.xlim([-r, r])
    plt.ylim([-r, r])

    plt.gca().set_aspect('equal')

def finalize(name):
    plt.legend()
    plt.savefig(f'{name}.pdf', bbox_inches='tight')
    plt.show()
