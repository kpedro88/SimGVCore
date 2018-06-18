# SimGVCore
Test package for GeantV in CMSSW

## Installation

Instructions:
```
cmsrel CMSSW_10_2_0_pre3
cd CMSSW_10_2_0_pre3
cmsenv
mkdir work
cd work
git clone git@github.com:kpedro88/install-geant.git -b CMSSW_10_1_ROOT612_X
ln -s install-geant/* .
./setup.sh -f -j 8 -I all >& log_setup.log
cd $CMSSW_BASE/src
cmsenv
git cms-merge-topic kpedro88:ImproveRootHandlers
git clone git@github.com:kpedro88/SimGVCore.git
scram b -j 8
```
(Important to `cmsenv` again after installation because of `scram setup` for new tools.)

Separate instructions for comparison tests w/ Geant4:
```
cmsrel CMSSW_10_2_0_pre3
cd CMSSW_10_2_0_pre3/src
cmsenv
git cms-merge-topic kpedro88:GVPhysicsList
git clone git@github.com:kpedro88/SimGVCore.git
cd SimGVCore
git config core.sparseCheckout true
echo "Application/test" > .git/info/sparse-checkout
git read-tree -mu HEAD
cd ..
scram b -j 8
```
(Separate area needed due to conflicts between different versions of external dependencies for Geant4 vs. GeantV in CMSSW.)

## Test suite

GEN parameters (`runGen.py`):
* `particle`: electron or photon
* `mult`: number of particles
* `pt`: transverse momentum of particles
* `maxEvents`: number of events

SIM parameters (`runSim.py`):
* above, plus
* `sim`: Geant4 or GeantV
* `year`: 2018 or 2023 (for CMS geometry)
* `threads`: number of threads
* `streams`: number of streams (0 -> `streams` = `threads`)

GEN and SIM are run separately so Geant4 and GeantV can process the exact same gen events and report only the CPU time used in simulation.

## Old tests

Test GEN-SIM together:
```
cd $CMSSW_BASE/src/SimGVCore/Application/test
cmsRun SingleElectronPt10_pythia8_cfi_GEN-SIM.py
```

Scaling test (GEN, SIM separately):
```
cd $CMSSW_BASE/src/SimGVCore/Application/test
cmsRun SingleElectronPt50_pythia8_cfi_GENonly.py
./testMT.sh >& log_test.log
./parseMT.sh log_test.log >& log_parsed.log
```
