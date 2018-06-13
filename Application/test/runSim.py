import FWCore.ParameterSet.Config as cms
from optGenSim import options

# eventually an era might be needed here
process = cms.Process('SIM')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
#process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Geometry.CMSCommonData.cmsExtendedGeometry2018NoSDXML_cfi')
#process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:'+options._genname+'.root')
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
    fileName = cms.untracked.string('file:'+options._simname+'.root'),
    outputCommands = process.RAWSIMEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

# Additional output definition

# Other statements

# selection of geometry (w/out SDs) based on year
if options.year==2018:
    process.load("SimGVCore.Application.cmsExtendedGeometry2018NoSDXML_cfi")
elif options.year==2023:
    process.load("SimGVCore.Application.cmsExtendedGeometry2023D17NoSDXML_cfi")

# modules for Geant4
if options.sim=="Geant4":
    # set a constant magnetic field
    process.load("MagneticField.Engine.uniformMagneticField_cfi")
    process.UniformMagneticFieldESProducer = cms.double(3.8)

    # load g4SimHits module and psim sequence
    process.load("Configuration.StandardSequences.SimIdeal_cff")

    # customize physics list to match GeantV
    process.g4SimHits.Physics.type = cms.string("SimG4Core/Physics/CMSEmGeantV")
    process.g4SimHits.Physics.DummyEMPhysics = cms.bool(False)

# modules for GeantV
elif options.sim=="GeantV":
    # this converts the geometry into TGeoManager for GeantV
    process.TGeoMgrFromDdd =  cms.ESProducer("TGeoMgrFromDdd",
        verbose = cms.untracked.bool(False),
        level = cms.untracked.int32(14)
    )

    process.geantv = cms.EDProducer("GeantVProducer",
        HepMCProductLabel = cms.InputTag("generatorSmeared"),
        geometry = cms.string(""),
        ZFieldInTesla = cms.double(3.8),
    )

    process.RAWSIMoutput.outputCommands.append("keep *_geantv_*_*")

    process.psim = cms.Sequence(process.geantv)

    process.MessageLogger.categories.append('GeantVProducer')
    process.MessageLogger.cerr.GeantVProducer = cms.untracked.PSet(
        optionalPSet = cms.untracked.bool(True),
        limit = cms.untracked.int32(10000000),
    )

# Path and EndPath definitions
process.simulation_step = cms.Path(process.psim)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RAWSIMoutput_step = cms.EndPath(process.RAWSIMoutput)

# Schedule definition
process.schedule = cms.Schedule(process.simulation_step,process.endjob_step,process.RAWSIMoutput_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)
# filter all path with the production filter sequence
for path in process.paths:
	getattr(process,path)._seq = process.generator * getattr(process,path)._seq 


# Customisation from command line

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion

process.options.numberOfThreads = cms.untracked.uint32(options.threads)
process.options.numberOfStreams = cms.untracked.uint32(options.streams)

from Validation.Performance.TimeMemoryInfo import customise
process = customise(process)
