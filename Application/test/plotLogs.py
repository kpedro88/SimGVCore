import sys, json
#import numpy as np
from cycler import cycler
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

if len(sys.argv)<2:
    sys.exit(1)

iname = sys.argv[1]

with open(iname,'r') as ifile:
    data = json.load(ifile)

# find available values in each parameter
params = ["sim","year","particle","pt","mult"]
ranges = {}
for param in params:
    ranges[param] = sorted(list(set([item[param] for item in data])))
#print ranges
qtys = ["time","rss"]

# separate plots per sim, per year, per particle, per qty
for sim in ranges["sim"]:
    for year in ranges["year"]:
        for particle in ranges["particle"]:
            for qty in qtys:
                # get the subset
                subdata = [item for item in data if item["sim"]==sim and item["year"]==year and item["particle"]==particle and qty in item]
                # find the normalization value
                try:
                    norm = min([item[qty] for item in subdata])
                except:
                    norm = 1
                fig1 = plt.figure(figsize=(7.5,5),dpi=100)
                ax1 = fig1.add_subplot(111)
                ax1.set_prop_cycle(cycler('color',['k','b','m','r']))
                # make a separate curve for each pt
                for pt in ranges["pt"]:
                    #print [sim,year,particle,qty,pt]
                    subdatapt = [item for item in subdata if item["pt"]==pt]
                    xy = map(list,zip(*sorted([(item["mult"],item[qty]/norm) for item in subdatapt])))
                    if len(xy)<2: continue
                    ptlabel = r"$p_{T}$ = "+"{:.0f}".format(pt)
                    ax1.plot(xy[0],xy[1],'-o',label=ptlabel)
                    #print xy
                ax1.set_xlabel("particle multiplicity")
                ax1.set_ylabel("Relative CPU usage")
                legend = ax1.legend(loc="upper right",numpoints=1)
                fig1.savefig("plot_"+sim+"_year"+str(year)+"_"+particle+"_"+qty+".png",dpi=100)
                    
