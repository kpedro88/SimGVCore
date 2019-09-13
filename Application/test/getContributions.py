from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter
from collections import OrderedDict
import json

def parseLogVal(line,item=""):
    if len(item)==0: return float(line.split(' ')[-1])
    else:
        linesplit = line.split(' ')
        if item in linesplit:
            return float(linesplit[linesplit.index(item)+1])
        else:
            return 0.

def getContributions(report, log, geant, ptype):
    geants = {
        "G4": "G4EventManager::DoProcessing",
        "GV": "geant::cxx::WorkloadManager::TransportTracksTask",
    }
    categories = OrderedDict([
        ("total","<spontaneous>"),
        ("geant",geants[geant]),
        ("scoring","update(sim::StepWrapper"),
        ("output","edm::PoolOutputModule::write"),
    ])
    stategories = [
        "time",
        "cpu",
        "vsize",
        "rss",
    ]

    contributions = OrderedDict()
    contributions["other"] = 0.0

    with open(report,'r') as infile:
        for line in infile:
            if line[0]=='[':
                for key,cat in categories.iteritems():
                    if cat in line and key not in contributions: # prevent overwriting
                        contributions[key] = float(line.split()[2].replace("'",""))
    # fill in missing
    for key in categories:
        if key not in contributions:
            contributions[key] = 0.0

    # compute remaining contribution (initialization + overhead) from total minus others
    # scoring is subset of geant, so skip it
    skipped = ["total","scoring"]
    contributions["other"] = contributions["total"] - sum([cont for key,cont in contributions.iteritems() if key not in skipped])

    # parse log for mem info and overall time
    stats = OrderedDict([(key,0.0) for key in stategories])
    if len(log)>0:
        with open(log,'r') as infile:
            for line in infile:
                # cleanup
                line = line.rstrip()
                if len(line)>0 and line[-1]==' ': line = line[:-1]
                if len(line)==0: continue

                elif "Total loop" in line:
                    if stats["time"]==0.0:
                        stats["time"] = parseLogVal(line)
                    else:
                        stats["cpu"] = parseLogVal(line)
                elif "VSIZE" in line or "RSS" in line:
                    if "VSIZE" in line:
                        stats["vsize"] = max(stats["vsize"],parseLogVal(line,"VSIZE"))
                    if "RSS" in line:
                        stats["rss"] = max(stats["rss"],parseLogVal(line,"RSS"))

    # print results
    results = OrderedDict([("contributions", contributions)])
    if len(log)>0: results["stats"] = stats
    if ptype=="text":
        for res,vals in results.iteritems():
            print res
            for key,cont in vals.iteritems():
                print "{0}: {1:.2f}".format(key,cont)
    elif ptype=="json":
        print json.dumps(results)

    return results

if __name__=="__main__":
    # check arguments
    parser = ArgumentParser(formatter_class=ArgumentDefaultsHelpFormatter)
    parser.add_argument("-g","--geant", dest="geant", type=str, default="", choices=["G4","GV"], help="geant version", required=True)
    parser.add_argument("-r", "--report", dest="report", type=str, default="", help="igprof report", required=True)
    parser.add_argument("-l", "--log", dest="log", type=str, default="", help="CMSSW output log")
    parser.add_argument("-p","--print", dest="ptype", type=str, default="text", choices=["","text","json"], help="how to print output")
    args = parser.parse_args()

    getContributions(args.report, args.log, args.geant, args.ptype)
