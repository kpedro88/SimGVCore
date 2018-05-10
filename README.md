# SimGVCore
Test package for GeantV in CMSSW

Installation instructions:
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
