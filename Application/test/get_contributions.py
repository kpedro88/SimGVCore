from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter
from collections import OrderedDict

# check arguments
parser = ArgumentParser(formatter_class=ArgumentDefaultsHelpFormatter)
parser.add_argument("-g","--geant", dest="geant", type=str, default="", choices=["G4","GV"], help="geant version", required=True)
parser.add_argument("-r", "--report", dest="report", type=str, default="", help="igprof report", required=True)
args = parser.parse_args()

geant = {
    "G4": "G4EventManager::DoProcessing",
    "GV": "geant::cxx::WorkloadManager::TransportTracksTask",
}
categories = OrderedDict([
    ("total","<spontaneous>"),
    ("geant",geant[args.geant]),
    ("scoring","update(sim::StepWrapper"),
    ("output","edm::PoolOutputModule::write"),
])

contributions = OrderedDict([(key,0.0) for key in categories])
contributions["other"] = 0.0

with open(args.report,'r') as infile:
    for line in infile:
        if line[0]=='[':
            for key,cat in categories.iteritems():
                if cat in line:
                    contributions[key] = float(line.split()[2])
                    
# compute remaining contribution (initialization + overhead) from total minus others
# scoring is subset of geant, so skip it
contributions["other"] = contributions["total"] - sum([cont for key,cont in contributions.iteritems() if key!="total" and key!="scoring"])

# print results
for key,cont in contributions.iteritems():
    print "{0}: {1:.2f}".format(key,cont)
