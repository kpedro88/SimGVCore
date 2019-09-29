import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

defaults = {
    "mineta": -3.,
    "maxeta": 3.,
    "minphi": -3.14159265359,
    "maxphi": 3.14159265359,
}

options = VarParsing("analysis")
options.register("particle", "electron", VarParsing.multiplicity.singleton, VarParsing.varType.string)
options.register("mult", 1, VarParsing.multiplicity.singleton, VarParsing.varType.int) # number of particles
options.register("energy", 1, VarParsing.multiplicity.singleton, VarParsing.varType.float)
options.register("mineta", defaults["mineta"], VarParsing.multiplicity.singleton, VarParsing.varType.float)
options.register("maxeta", defaults["maxeta"], VarParsing.multiplicity.singleton, VarParsing.varType.float)
options.register("minphi", defaults["minphi"], VarParsing.multiplicity.singleton, VarParsing.varType.float)
options.register("maxphi", defaults["maxphi"], VarParsing.multiplicity.singleton, VarParsing.varType.float)
options.register("sim", "G4", VarParsing.multiplicity.singleton, VarParsing.varType.string)
options.register("threads", 1, VarParsing.multiplicity.singleton, VarParsing.varType.int)
options.register("streams", 0, VarParsing.multiplicity.singleton, VarParsing.varType.int)
options.register("year", 2018, VarParsing.multiplicity.singleton, VarParsing.varType.int)
options.register("bfield", 3.8, VarParsing.multiplicity.singleton, VarParsing.varType.float)
options.register("maxEventsIn", -1, VarParsing.multiplicity.singleton, VarParsing.varType.int)
options.register("output", True, VarParsing.multiplicity.singleton, VarParsing.varType.bool)
options.register("ncopy", 1, VarParsing.multiplicity.singleton, VarParsing.varType.int)
options.parseArguments()

# choose particle
options._pdgid = 0
if options.particle=="electron": options._pdgid = 11
elif options.particle=="photon": options._pdgid = 22
else: raise ValueError("Unsupported particle: "+options.particle)

# check options
sims = ["G4","GV","GVst"]
if options.streams == 0: options.streams = options.threads
if options.sim not in sims:
    raise ValueError("Unsupported sim: "+options.sim)
if options.year!=2018 and options.year!=2023:
    raise ValueError("Unsupported year: "+str(options.year))
if options.maxEventsIn==-1: options.maxEventsIn = options.maxEvents

# basic name definition
nametmp = options.particle+"_energy"+str(options.energy)+"_mult"+str(options.mult)
if any([defaults[x] != getattr(options,x) for x in defaults]):
    nametmp = nametmp+"_eta"+str(options.mineta)+"to"+str(options.maxeta)+"_phi"+str(options.minphi)+"to"+str(options.maxphi)
# gen name definition
options._genname = "gen_"+nametmp+"_n"+str(options.maxEventsIn)
# copy name definition (concatenated gen files)
options._copyname = "gen_"+nametmp+"_n"+str(options.maxEventsIn*options.ncopy)
# sim name definition
options._simname = "sim_"+options.sim+"_year"+str(options.year)+"_"+nametmp+"_bfield"+str(options.bfield)+"_n"+str(options.maxEvents)+"_th"+str(options.threads)+"_st"+str(options.streams)
