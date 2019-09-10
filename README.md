# SimGVCore
Test package for GeantV in CMSSW

## Installation

Instructions:
```
wget https://raw.githubusercontent.com/kpedro88/SimGVCore/SensDetTemplateWrapper/setup.sh
chmod +x setup.sh
./setup.sh -s GV,G4
```

This will create a GV directory with CMSSW and GeantV installed, and a G4 directory with CMSSW and Geant4 installed.
(Separate areas are needed due to conflicts between different versions of external dependencies for Geant4 vs. GeantV in CMSSW.)

## Test suite

GEN parameters (`runGen.py`):
* `particle`: electron or photon
* `mult`: number of particles
* `energy`: energy of particles
* `maxEvents`: number of events

SIM parameters (`runSim.py`):
* above, plus
* `sim`: G4 or GV
* `year`: 2018 or 2023 (for CMS geometry)
* `threads`: number of threads
* `streams`: number of streams (0 -> `streams` = `threads`)
* `maxEventsIn`: number of input events (in case running over subset of generated events)

GEN and SIM are run separately so Geant4 and GeantV can process the exact same gen events and report only the CPU time used in simulation.
