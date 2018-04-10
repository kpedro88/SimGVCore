# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: SingleElectronPt10_pythia8_cfi --conditions auto:phase1_2018_realistic -n 10 --era Run2_2018 --eventcontent RAWSIM -s GEN --datatier GEN --beamspot Realistic25ns13TeVEarly2017Collision --geometry DB:Extended --no_exec --fileout file:step1.root
import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing("analysis")
options.register("nThreads",1,VarParsing.multiplicity.singleton,VarParsing.varType.int)
options.parseArguments()

from Configuration.StandardSequences.Eras import eras

process = cms.Process('SIM',eras.Run2_2018)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
#process.load('SimGeneral.MixingModule.mixNoPU_cfi')
#process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
#process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:step1.root')
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('SingleElectronPt10_pythia8_cfi nevts:10'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.RAWSIMoutput = cms.OutputModule("PoolOutputModule",
    compressionAlgorithm = cms.untracked.string('LZMA'),
    compressionLevel = cms.untracked.int32(9),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('GEN-SIM'),
        filterName = cms.untracked.string('')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(20971520),
    fileName = cms.untracked.string('file:step2.root'),
    outputCommands = process.RAWSIMEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)
process.RAWSIMoutput.outputCommands.append("keep *_geantv_*_*")

# Additional output definition

# Other statements
process.geantv = cms.EDProducer("GeantVProducer",
    HepMCProductLabel = cms.InputTag("generatorSmeared"),
    geometry = cms.string("/uscms_data/d3/pedrok/geant/files/cms2018.gdml"),
)

process.sim = cms.Sequence(process.geantv)

process.MessageLogger.categories.append('GeantVProducer')
process.MessageLogger.cerr.GeantVProducer = cms.untracked.PSet(
    optionalPSet = cms.untracked.bool(True),
    limit = cms.untracked.int32(10000000),
)

# Path and EndPath definitions
process.simulation_step = cms.Path(process.sim)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RAWSIMoutput_step = cms.EndPath(process.RAWSIMoutput)

# Schedule definition
process.schedule = cms.Schedule(process.simulation_step,process.endjob_step,process.RAWSIMoutput_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

# Customisation from command line

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion

process.options.numberOfThreads = cms.untracked.uint32(options.nThreads)
process.options.numberOfStreams = cms.untracked.uint32(0)

from Validation.Performance.TimeMemoryInfo import customise
process = customise(process)
