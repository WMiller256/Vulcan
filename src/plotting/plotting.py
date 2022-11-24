import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import matplotlib.font_manager
import numpy as np
import sys

from math import log10, floor
from matplotlib import rcParams
from matplotlib.ticker import FormatStrFormatter

import os.path

plt.style.use('miller')

def exp(number) -> int:
    return abs(floor(log10(abs(number))))
    
def plot(name, threads):
    colors = ["#0040a8", "#00a835", "#d7db0d", "#960500", "#960050", "#09b6ba"]
    
    ith = 0
    outpath = "/mnt/c/Users/words/eclipse/Thesis/"
    outname = "Vulcan_Figure-5.pdf"
    
    plt.xlabel("Number of bodies")
    plt.ylabel("Computation Time in Milliseconds")

    log = plt.gca()
    log.set_xscale('log')
    log.set_yscale('log')
    log.tick_params(axis='both', which='major', direction='in')
    log.tick_params(axis='x', which='minor', bottom=False)
    log.xaxis.set_major_formatter(ticker.FuncFormatter(lambda y, _ : f'{exp(y)}'))

    if name != "All":
        plt.title(name+" Threading", fontsize = 24)

        for ii in range(1,threads+1):
            x,y = read("../../data/BS-threadTesting.2.txt", name.lower(), ii)
            lin.plot(x,y, label = str(ii), c = colors[ii%len(colors)])
            log.plot(x, y, c = colors[ii%len(colors)])
        legend = plt.figlegend(prop={'size': 20}, title = "Threads", fontsize = 20, fancybox = True, loc = "upper left", bbox_to_anchor=(0.075,0.95))
        plt.setp(legend.get_title(),fontsize = 20)
        plt.savefig(outpath+outname, bbox_inches="tight")
    else:
        plt.title("Measured improvement of parallelization")
        for handle, label in zip(["omp", 'explicit', 'single'], ['Intra-step', 'Inter-step', 'Serial']):
            x,y = read("../../data/BS-threadTesting.2.txt", handle, threads)
            log.plot(np.array(x) * 1e2, y, label=label, linewidth=2)
        
        plt.legend(prop={'size': 20}, title = "Method", fontsize=20, fancybox=True)
        plt.savefig(outpath+outname, bbox_inches="tight")
        plt.show()
        
    
def read(filename, mode, threads):
    with open(filename) as fp:
        lines = fp.readlines();

    x = list()
    y = list()
    for ii in range(len(lines)):
        temp = lines[ii].split()
        if len(temp) > 3:
            if temp[0] == mode and int(temp[3]) == threads:
                x.append(int(temp[1]))
                y.append(int(temp[2]))
    return x,y

plot("All", 6)
