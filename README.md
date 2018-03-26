# SimGVCore
Test package for GeantV in CMSSW

Installation instructions:
```
scram list CMSSW_10_1_ROOT612 | tail -n 2 | head -n 1 | awk '{print $2}' > CMSSW_LATEST
cmsrel `cat CMSSW_LATEST`
cd `cat CMSSW_LATEST`
cmsenv
mkdir work
cd work
git clone git@github.com:kpedro88/install-geant.git -b CMSSW_10_1_ROOT612_X
ln -s install-geant/* .
./setup.sh -f -j 8 -I all >& log_setup.log
cd $CMSSW_BASE/src
cmsenv
git clone git@github.com:kpedro88/SimGVCore.git
scram b -j 8

```

(Important to `cmsenv` again after installation because of `scram setup` for new tools.)
