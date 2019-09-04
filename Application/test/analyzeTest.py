import sys, os, json
from getContributions import getContributions
from collections import OrderedDict
from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter

# check arguments
parser = ArgumentParser(formatter_class=ArgumentDefaultsHelpFormatter)
parser.add_argument("-t", "--test", dest="test", type=int, help="test number", required=True)
args = parser.parse_args()

testdir = "test"+str(args.test)
if not os.path.isdir(testdir):
    parser.error("Directory "+testdir+" does not exist")

# clear sys args before importing optGenSim (VarParsing always tries to read them)
sys.argv[1:] = []
import SimGVCore.Application.optGenSim as gs

# loop over all tests in this dir
all_results = []
with open(testdir+"/args.txt",'r') as argfile:
    for line in argfile:
        # parse args for this test
        sys.argv[1:] = line.rstrip().split(' ')
        reload(gs)

        # parse log and report
        results = getContributions(
            report="igreport_"+gs.options._simname+".res",
            log="igprof_"+gs.options._simname+".res",
            geant=gs.options.sim,
            ptype="",
        )

        # get parameters
        results["parameters"] = OrderedDict([
            "particle": gs.options.particle,
            "mult": gs.options.mult,
            "energy": gs.options.energy,
            "sim": gs.options.sim,
            "threads": gs.options.threads,
            "streams": gs.options.streams,
            "maxEvents": gs.options.maxEvents,
        ])

        all_results.append(results)

# dump output for later plotting    
with open("data.json",'w') as outfile:
	json.dump(all_results,outfile)
