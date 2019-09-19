import FWCore.ParameterSet.Config as cms
from SimGVCore.Application.optGenSim import options

process = cms.Process('COPY')

# based on PhysicsTools/Utilities/configuration/copyPickMerge_cfg.py

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(['file:'+options._genname+'.root']*options.ncopy),
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1),
)

process.Out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('file:'+options._copyname+'.root'),
)

process.end = cms.EndPath(process.Out)
