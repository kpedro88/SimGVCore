import os, sys
from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter
from collections import OrderedDict
import numpy as np
import pandas as pd
from cycler import cycler
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt

# mplhep for cms style
sys.path.append(os.getcwd()+'/.local/lib/python2.7/site-packages/')
import mplhep as hep
# avoid incompatibilities
for x in list(hep.cms.style.ROOT.keys()):
    if x not in plt.rcParams.keys(): del hep.cms.style.ROOT[x]
plt.style.use(hep.cms.style.ROOT)
plt.style.use('default.mplstyle')

# check arguments
parser = ArgumentParser(formatter_class=ArgumentDefaultsHelpFormatter)
parser.add_argument("-t", "--test", dest="test", type=str, help="test name", required=True)
parser.add_argument("-x", dest="x", type=str, help="x variable", required=True)
parser.add_argument("-y", dest="y", type=str, default=["cpu","mem","time"], help="y variable(s)", nargs='*')
parser.add_argument("-z", dest="z", type=str, help="comparison variable", required=True)
parser.add_argument("-c", "--cut", dest="cut", type=str, default="", help="cut on params")
parser.add_argument("-i", "--ignore", dest="ignore", type=str, default=[], help="params to ignore", nargs='*')
parser.add_argument("-n", "--numer", dest="numer", type=str, default=[], help="numer(s) for ratio", nargs='*')
parser.add_argument("-d", "--denom", dest="denom", type=str, default=[], help="denom(s) for ratio", nargs='*')
parser.add_argument("-s", "--suffix", dest="suffix", type=str, default="", help="suffix for plots")
parser.add_argument("-f","--formats", dest="formats", type=str, default=["png"], nargs='*', help="print plots in specified format(s)")
parser.add_argument("-p","--perthread", dest="perthread", default=False, action="store_true", help="normalize by # of threads")
args = parser.parse_args()

# plot categories
plot_qtys = OrderedDict([
    ("cpu",["total","other","geant","scoring","output"]),
    ("mem",["vsize","rss"]),
    ("vsize",["vsize"]),
    ("rss",["rss"]),
    ("time",["time"]),
    ("throughput",["throughput"]),
])
ytitles = {
    "cpu": "CPU [ticks]",
    "mem": "memory [MB]",
    "time": "wall clock time [s]",
    "throughput": "throughput [evt/s]",
    "vsize": "virtual memory [MB]",
    "rss": "RSS memory [MB]",
}
col_qtys = OrderedDict()
col_qtys["throughput"] = "stats"

# additions for derived speedup
if args.x=="threads" and any(q in args.y for q in ["time","throughput"]):
    plot_qtys["speedup"] = ["speedup"]
    args.y.append("speedup")
    ytitles["speedup"] = "speedup"
    col_qtys["speedup"] = "stats"

ratio_allowed = (len(args.numer)==1 and len(args.denom)==1) or \
                (len(args.numer)==1 and len(args.denom)>1) or \
                (len(args.numer)>1 and len(args.denom)==1) or \
                (len(args.numer)>0 and len(args.numer)==len(args.denom))
if not ratio_allowed and (len(args.numer)>0 or len(args.denom)>0):
    parser.error("Inconsistent ratio arguments")

testdir = "test"+args.test
if not os.path.isdir(testdir):
    parser.error("Directory "+testdir+" does not exist")
datafile = testdir+"/data.json"
if not os.path.isfile(datafile):
    parser.error("Test "+str(args.test)+" has not been analyzed (data.json missing)")

with open(datafile,'r') as dfile:
    dframe = pd.read_json(dfile,orient='records')
# this produces a data frame with three parts: 
# parameters, contributions, stats
dframe.columns = pd.MultiIndex.from_tuples([(x.split('_')[0],x.split('_')[1]) for x in dframe.columns])

# get top level column for each qty
for col in ["contributions","stats"]:
    for key in dframe[col]:
        col_qtys[key] = col

# apply cut if any
# not sure how to query entire multi index dataframe, so just query subsection and make a loc mask
if len(args.cut)>0:
    dframe = dframe.loc[dframe["parameters"].query(args.cut).index]
    
# get values of comparison variable
zvals = sorted(list(dframe["parameters"][args.z].unique()))
zframes = OrderedDict()

# get dframe subsets
ignore_list = args.ignore[:]
ignore_list.append(args.x)
for zval in zvals:
    zframes[zval] = dframe.loc[dframe["parameters"].query(args.z+" == '"+str(zval)+"'").index].copy()
    # adjust index
    zframes[zval].index = range(0,len(zframes[zval]))
    # check for unwanted parameter variations
    params = [str(col) for col in zframes[zval]["parameters"]]
    # omit x var from check
    nonuniqs = [p for p in params if p not in ignore_list and len(zframes[zval]["parameters"][p].unique())>1]
    if len(nonuniqs)>0:
        raise ValueError("Dataframe subset for z = "+str(zval)+" has varied parameters: "+",".join(nonuniqs))
    # calculate speedup if varying # threads
    if args.perthread:
        zframes[zval][("stats","throughput")] = pd.Series(zframes[zval]["parameters"]["maxEvents"]).divide(zframes[zval]["stats"]["time"])
        for stat in ["time","cpu"]:
            zframes[zval][("stats",stat)] = pd.Series(zframes[zval]["stats"][stat]).divide(zframes[zval]["parameters"]["threads"])
    if args.x=="threads":
        zframes[zval][("stats","speedup")] = np.power(pd.Series(zframes[zval]["stats"]["time"])/zframes[zval].loc[zframes[zval]["parameters"].query("threads==1").index]["stats"]["time"][0],-1)

# compute ratios
rframes = OrderedDict()
ratio_title = ""
ratio_key = 1
def make_ratio(numer,denom,rframes,zframes):
    rframes[(numer,denom)] = zframes[numer].copy()
    # don't divide params
    for col in ["contributions","stats"]:
        rframes[(numer,denom)][col] /= zframes[denom][col]
if len(args.numer)==1 and len(args.denom)==1:
    make_ratio(args.numer[0],args.denom[0],rframes,zframes)
    ratio_title = args.numer[0]+" / "+args.denom[0]
elif len(args.numer)==1 and len(args.denom)>1:
    for denom in args.denom:
        make_ratio(args.numer[0],denom,rframes,zframes)
    ratio_title = args.numer[0]+" / *"
elif len(args.numer)>1 and len(args.denom)==1:
    for numer in args.numer:
        make_ratio(numer,args.denom[0],rframes,zframes)
    ratio_title = "* / "+args.denom[0]
    ratio_key = 0
elif len(args.numer)>0 and len(args.numer)==len(args.denom):
    for numer,denom in zip(args.numer,args.denom):
        make_ratio(numer,denom,rframes,zframes)
    ratio_title = "ratio"

# plotting helper functions
def prop_repr(prop, val):
    prop_names = {
        "bfield": "Field",
        "energy": "Energy",
        "mult": "N",
        "particle": "Particle"
    }
    prop_vals = {
        "bfield": "{:.1f} T",
        "energy": "{:d} GeV",
    }
    line = (prop_names[prop] if prop in prop_names else prop)+": "+(prop_vals[prop].format(val) if prop in prop_vals else str(val))
    return line
def make_plot():
    fig, (ax, lax) = plt.subplots(ncols=2, gridspec_kw={"width_ratios":[2.5,1]})
    ax.set_prop_cycle(cycler('color',['k','b','m','r','c']))
    ax = hep.cms.cmslabel(ax, data=False, paper=False, rlabel="")
    return fig, ax, lax
def make_legend(ax, lax, frame, ignore_list):
    # add params to legend
    handles, labels = ax.get_legend_handles_labels()
    patches = [mpl.patches.Patch(color='w', label=prop_repr(p,frame["parameters"][p][0])) for p in frame["parameters"] if p not in ignore_list]
    handles.extend(patches)
    labels.extend([patch.get_label() for patch in patches])
    legend = lax.legend(handles=handles, labels=labels, borderaxespad=0, loc='upper right')
    # remove unwanted components
    lax.axis("off")
    ax.get_legend().remove()
def save_plot(fig, name, formats):
    for format in args.formats:
        fargs = {}
        if format=="png": fargs = {"dpi":100}
        elif format=="pdf": fargs = {"bbox_inches":"tight"}
        fig.savefig(name+"."+format,**fargs)

for plot in args.y:
    qtys = plot_qtys[plot]
    # everything separate
    if len(qtys)>1:
        vals = zframes.keys()
        if len(rframes)>0: vals.extend(rframes.keys())
        for val in vals:
            if isinstance(val, tuple): 
                frame = rframes[val]
                isratio = True
                this_ratio_title = val[0] + " / "+ val[1]
            else:
                frame = zframes[val]
                isratio = False

            fig, ax, lax = make_plot()

            # make line plots
            for qty in qtys:
                frame.plot.line(x=("parameters",args.x), y=(col_qtys[qty],qty), label=qty, ax=ax)

            # axis info
            ax.set_xlabel(args.x)
            ax.set_ylabel(this_ratio_title+" ("+ytitles[plot]+")" if isratio else ytitles[plot])
            fig.tight_layout()

            ignore_list2 = ignore_list[:]
            if isratio: ignore_list2.append(args.z)
            make_legend(ax, lax, frame, ignore_list2)

            # print
            pname = args.x+"_vs_"+plot+"__"+("ratio" if isratio else val)
            if len(args.suffix)>0: pname = pname+"__"+args.suffix
            fname = testdir+"/"+pname
            save_plot(fig, fname, args.formats)
    # sims together and ratios together
    else:
        qty = qtys[0]
        allframes = [zframes]
        colors = OrderedDict()
        if len(rframes)>0: allframes.append(rframes)
        for frames in allframes:
            fig, ax, lax = make_plot()

            # make line plots
            isratio = False
            for val in frames:
                if isinstance(val,tuple):
                    label = val[0] + " / " + val[1]
                    isratio = True
                else:
                    label = val
                frames[val].plot.line(x=("parameters",args.x), y=(col_qtys[qty],qty), label=label, ax=ax)
                line = ax.get_lines()[-1]
                if isratio:
                    line.set_color(colors[val[ratio_key]])
                else:
                    colors[val] = line.get_color()

            # axis info
            ax.set_xlabel(args.x)
            ax.set_ylabel(ratio_title+" ("+ytitles[plot]+")" if isratio else ytitles[plot])
            fig.tight_layout()

            ignore_list2 = ignore_list[:]
            ignore_list2.append(args.z)
            make_legend(ax, lax, frames.items()[0][1], ignore_list2)

            # print
            pname = args.x+"_vs_"+plot+"__"+("ratio" if isratio else args.z)
            if len(args.suffix)>0: pname = pname+"__"+args.suffix
            fname = testdir+"/"+pname
            save_plot(fig, fname, args.formats)
