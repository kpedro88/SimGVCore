import sys, os, json
from getContributions import getContributions
from collections import OrderedDict
from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter

def analyzeFile(dir, fname, all_results):
    with open(dir+"/"+fname,'r') as argfile:
        for line in argfile:
            # parse args for this test
            sys.argv[1:] = line.rstrip().split(' ')
            reload(gs)

            # parse log and report
            results = getContributions(
                report=dir+"/igreport_"+gs.options._simname+".res",
                log=dir+"/log_"+gs.options._simname+".log",
                geant=gs.options.sim,
                ptype="",
            )

            # get parameters
            results["parameters"] = OrderedDict([
                ("particle", gs.options.particle),
                ("mult", gs.options.mult),
                ("energy", gs.options.energy),
                ("bfield", gs.options.bfield),
                ("sim", gs.options.sim),
                ("threads", gs.options.threads),
                ("streams", gs.options.streams),
                ("maxEvents", gs.options.maxEvents),
            ])

            # transform into multi-index friendly format
            results2 = OrderedDict()
            for col in results:
                for param in results[col]:
                    results2[col+"_"+param] = results[col][param]

            all_results.append(results2)

# check arguments
parser = ArgumentParser(formatter_class=ArgumentDefaultsHelpFormatter)
parser.add_argument("-t", "--test", dest="test", type=str, help="test name", required=True)
args = parser.parse_args()

testdir = "test"+args.test
if not os.path.isdir(testdir):
    parser.error("Directory "+testdir+" does not exist")

# clear sys args before importing optGenSim (VarParsing always tries to read them)
sys.argv[1:] = []
import SimGVCore.Application.optGenSim as gs

# loop over all tests in this dir
all_results = []
analyzeFile(testdir, "args.txt", all_results)
# check for link to alternate sim dir for corresponding test
for sim in ["G4","GV"]:
    altdir = sim+"/"+testdir
    if os.path.isdir(altdir):
        analyzeFile(altdir, "args.txt", all_results)

# dump output for later plotting    
with open(testdir+"/data.json",'w') as outfile:
	json.dump(all_results,outfile)
