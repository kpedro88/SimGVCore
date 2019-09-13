import os
from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter
from collections import OrderedDict
import pandas as pd
from cycler import cycler
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt
plt.style.use('default.mplstyle')

# check arguments
parser = ArgumentParser(formatter_class=ArgumentDefaultsHelpFormatter)
parser.add_argument("-t", "--test", dest="test", type=str, help="test name", required=True)
parser.add_argument("-x", dest="x", type=str, help="x variable", required=True)
parser.add_argument("-z", dest="z", type=str, help="comparison variable", required=True)
parser.add_argument("-c", "--cut", dest="cut", type=str, default="", help="cut on params")
parser.add_argument("-i", "--ignore", dest="ignore", type=str, default=[], help="params to ignore", nargs='*')
parser.add_argument("-n", "--numer", dest="numer", type=str, default="", help="numer for ratio")
parser.add_argument("-d", "--denom", dest="denom", type=str, default="", help="denom for ratio")
parser.add_argument("-s", "--suffix", dest="suffix", type=str, default="", help="suffix for plots")
parser.add_argument("-f","--formats", dest="formats", type=str, default=["png"], nargs='*', help="print plots in specified format(s)")
args = parser.parse_args()

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
    zframes[zval] = dframe.loc[dframe["parameters"].query(args.z+" == '"+str(zval)+"'").index]
    # adjust index
    zframes[zval].index = range(0,len(zframes[zval]))
    # check for unwanted parameter variations
    params = [str(col) for col in zframes[zval]["parameters"]]
    # omit x var from check
    nonuniqs = [p for p in params if p not in ignore_list and len(zframes[zval]["parameters"][p].unique())>1]
    if len(nonuniqs)>0:
        raise ValueError("Dataframe subset for z = "+str(zval)+" has varied parameters: "+",".join(nonuniqs))

# compute ratios
zframes["ratio"] = None
ratiotitle = ""
if len(args.numer)>0 and len(args.denom)>0:
    zframes["ratio"] = zframes[args.numer].copy()
    ## adjust index so division will work
    #zframes["ratio"].index = zframes[args.denom].index
    # don't divide params
    for col in ["contributions","stats"]:
        zframes["ratio"][col] /= zframes[args.denom][col]
    ratiotitle = args.numer+" / "+args.denom

# plot categories
plot_qtys = OrderedDict([
    ("cpu",["total","other","geant","scoring","output"]),
    ("mem",["vsize","rss"]),
    ("time",["time"]),
])
ytitles = {
    "cpu": "CPU [ticks]",
    "mem": "memory [MB]",
    "time": "wall clock time [s]",
}

# get top level column for each qty
col_qtys = OrderedDict()
for col in ["contributions","stats"]:
    for key in dframe[col]:
        col_qtys[key] = col

for plot in plot_qtys:
    for zval,zframe in zframes.iteritems():
        fig, (ax, lax) = plt.subplots(ncols=2, gridspec_kw={"width_ratios":[2.5,1]})
        ax.set_prop_cycle(cycler('color',['k','b','m','r','c']))
        
        # make line plots
        for qty in plot_qtys[plot]:
            zframe.plot.line(x=("parameters",args.x), y=(col_qtys[qty],qty), label=qty, ax=ax)
            
        # axis info
        ax.set_xlabel(args.x)
        ax.set_ylabel(ratiotitle+" ("+ytitles[plot]+")" if zval=="ratio" else ytitles[plot])
        fig.tight_layout()
        
        # add params to legend
        handles, labels = ax.get_legend_handles_labels()
        ignore_list2 = ignore_list[:]
        if zval=="ratio": ignore_list2.append(args.z)
        patches = [mpl.patches.Patch(color='w', label=p+": "+str(zframe["parameters"][p][0])) for p in zframe["parameters"] if p not in ignore_list2]
        handles.extend(patches)
        labels.extend([patch.get_label() for patch in patches])
        legend = lax.legend(handles=handles, labels=labels, borderaxespad=0, loc='upper right')
        # remove unwanted components
        lax.axis("off")
        ax.get_legend().remove()
        
        # print
        pname = args.x+"_vs_"+plot+"__"+zval
        if len(args.suffix)>0: pname = pname+"__"+args.suffix
        fname = testdir+"/"+pname
        for format in args.formats:
            fargs = {}
            if format=="png": fargs = {"dpi":100}
            elif format=="pdf": fargs = {"bbox_inches":"tight"}
            fig.savefig(fname+"."+format,**fargs)
