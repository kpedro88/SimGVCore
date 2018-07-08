import sys, json

def parseLogName(log):
    logsplit = log.replace(".log","").split("_")
    item = {}
    item["sim"] = logsplit[1]
    item["year"] = int(logsplit[2][4:])
    item["particle"] = logsplit[3]
    item["mult"] = int(logsplit[4][4:])
    item["pt"] = float(logsplit[5][2:])
    item["nevents"] = int(logsplit[6][1:])
    return item

def parseReportVal(line):
    return float(line.split(' ')[-1])

if len(sys.argv)<3:
    sys.exit(1)

oname = sys.argv[1]
fnames = sys.argv[2:]

data = []

for fname in fnames:
    with open(fname,'r') as file:
        item = {}
        for line in file:
            #cleanup
            line = line.rstrip()
            if line[-1]==' ': line = line[:-1]

            if "log" in line:
                # store previous item
                if len(item)>0: data.append(item)
                # start new item
                item = parseLogName(line)
            elif "Total loop" in line:
                if "time" not in item:
                    item["time"] = parseReportVal(line)
                else:
                    item["cpu"] = parseReportVal(line)
            elif "VSIZE" in line:
                item["vsize"] = parseReportVal(line)
            elif "RSS" in line:
                item["rss"] = parseReportVal(line)
#            print item
        if len(item)>0: data.append(item)

with open(oname,'w') as ofile:
    json.dump(data,ofile,indent=2)
