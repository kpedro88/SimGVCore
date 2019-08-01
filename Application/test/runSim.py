import FWCore.ParameterSet.Config as cms
from SimGVCore.Application.optGenSim import options

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

# scoring params
scoring_ = cms.PSet(
    type         = cms.string('CaloSteppingAction'),
    CaloSteppingAction = cms.PSet(
        EBSDNames       = cms.vstring('EBRY'),
        EESDNames       = cms.vstring('EFRY'),
        HCSDNames       = cms.vstring('HBS','HES','HTS'),
        AllSteps        = cms.int32(0),
        SlopeLightYield = cms.double(0.02),
        BirkC1EC        = cms.double(0.03333),
        BirkSlopeEC     = cms.double(0.253694),
        BirkCutEC       = cms.double(0.1),
        BirkC1HC        = cms.double(0.0052),
        BirkC2HC        = cms.double(0.142),
        BirkC3HC        = cms.double(1.75),
        HitCollNames = cms.vstring('EcalHitsEB1','EcalHitsEE1','HcalHits1'),
        EtaTable        = cms.vdouble(0.000, 0.087, 0.174, 0.261, 0.348,
                                      0.435, 0.522, 0.609, 0.696, 0.783,
                                      0.870, 0.957, 1.044, 1.131, 1.218,
                                      1.305, 1.392, 1.479, 1.566, 1.653,
                                      1.740, 1.830, 1.930, 2.043, 2.172,
                                      2.322, 2.500, 2.650, 2.868, 3.000),
        PhiBin          = cms.vdouble(5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0,
                                      5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0,
                                      5.0, 5.0, 5.0, 5.0, 5.0, 5.0,10.0,
                                     10.0,10.0,10.0,10.0,10.0,10.0,10.0,
                                     10.0),
        PhiOffset        = cms.vdouble( 0.0, 0.0, 0.0,10.0),
        EtaMin           = cms.vint32(1, 16, 29, 1),
        EtaMax           = cms.vint32(16, 29, 41, 15),
        EtaHBHE          = cms.int32(16),
        DepthHBHE        = cms.vint32(2,4),
        Depth29Max       = cms.int32(3),
        RMinHO           = cms.double(3800),
        ZHO              = cms.vdouble(0,1255,1418,3928,4100,6610),
        Eta1             = cms.untracked.vint32(1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                1, 1, 1, 1, 1, 1, 1, 4, 4),
        Eta15            = cms.untracked.vint32(1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                1, 1, 1, 2, 2, 2, 2, 4, 4),
        Eta16            = cms.untracked.vint32(1, 1, 2, 2, 2, 2, 2, 2, 2, 4,
                                                4, 4, 4, 4, 4, 4, 4, 4, 4),
        Eta17            = cms.untracked.vint32(2, 2, 2, 2, 2, 2, 2, 2, 3, 3,
                                                3, 3, 3, 3, 3, 3, 3, 3, 3),
        Eta18            = cms.untracked.vint32(1, 2, 2, 2, 3, 3, 3, 3, 4, 4,
                                                4, 5, 5, 5, 5, 5, 5, 5, 5),
        Eta19            = cms.untracked.vint32(1, 1, 2, 2, 2, 3, 3, 3, 4, 4,
                                                4, 5, 5, 5, 5, 6, 6, 6, 6),
        Eta26            = cms.untracked.vint32(1, 1, 2, 2, 3, 3, 4, 4, 5, 5,
                                                5, 6, 6, 6, 6, 7, 7, 7, 7),
    ),
)

# modules for Geant4
if options.sim=="Geant4":
    # set a constant magnetic field
    process.load("MagneticField.Engine.uniformMagneticField_cfi")
    process.UniformMagneticFieldESProducer.ZFieldInTesla = cms.double(3.8)

    # load g4SimHits module and psim sequence
    process.load("Configuration.StandardSequences.SimIdeal_cff")

    # customize physics list to match GeantV
    process.g4SimHits.Physics.type = cms.string("SimG4Core/Physics/DummyPhysics")
    process.g4SimHits.Physics.DummyEMPhysics = cms.bool(False)
    process.g4SimHits.Watchers = cms.VPSet(scoring_)
    process.g4SimHits.Physics.CutsPerRegion = False
    process.g4SimHits.Physics.CutsOnProton  = False
    process.g4SimHits.Physics.DefaultCutValue = 0.1
    process.g4SimHits.StackingAction.TrackNeutrino = True
    process.g4SimHits.StackingAction.MaxTrackTime = 2000.0
    process.g4SimHits.StackingAction.MaxTrackTimes = []
    process.g4SimHits.StackingAction.MaxTimeNames = []
    process.g4SimHits.StackingAction.DeadRegions = []
    process.g4SimHits.StackingAction.CriticalEnergyForVacuum = 0
    process.g4SimHits.StackingAction.KillGamma     = False
    process.g4SimHits.StackingAction.RusRoGammaEnergyLimit = 0
    process.g4SimHits.StackingAction.RusRoNeutronEnergyLimit = 0
    process.g4SimHits.SteppingAction.MaxTrackTime = 2000.0
    process.g4SimHits.SteppingAction.MaxTrackTimes = []
    process.g4SimHits.SteppingAction.MaxTimeNames = []
    process.g4SimHits.SteppingAction.DeadRegions = []
    process.g4SimHits.SteppingAction.CriticalEnergyForVacuum = 0
    process.g4SimHits.MagneticField.ConfGlobalMFM.OCMS.StepperParam.MinimumEpsilonStep = 0.001
    process.g4SimHits.MagneticField.ConfGlobalMFM.OCMS.StepperParam.MaximumEpsilonStep = 0.001
    process.g4SimHits.MagneticField.ConfGlobalMFM.OCMS.StepperParam.EnergyThSimple = 0
    process.g4SimHits.MagneticField.ConfGlobalMFM.OCMS.StepperParam.DeltaChordSimple = 0.001
    process.g4SimHits.MagneticField.ConfGlobalMFM.OCMS.StepperParam.DeltaOneStepSimple = 0.001
    process.g4SimHits.MagneticField.ConfGlobalMFM.OCMS.StepperParam.DeltaIntersectionSimple = 0.0001
    process.MessageLogger.categories.append('Step')

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
        Scoring = scoring_,
    )

    process.RAWSIMoutput.outputCommands.append("keep *_geantv_*_*")

    process.psim = cms.Sequence(process.geantv)

    process.MessageLogger.categories.append('GeantVProducer')
    process.MessageLogger.categories.append('Step')
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


# Customisation from command line

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion

process.options.numberOfThreads = cms.untracked.uint32(options.threads)
process.options.numberOfStreams = cms.untracked.uint32(options.streams)

from Validation.Performance.TimeMemoryInfo import customise
process = customise(process)
