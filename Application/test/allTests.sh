#!/bin/bash

./testStandalone.sh GV >& log_test_standalone_gv.log
echo "finished GV standalone"
./testStandalone.sh G4 >& log_test_standalone_g4.log
echo "finished G4 standalone"
./testMT.sh -t MM2 -s GV -n 500 >& log_testMM2.log
echo "finished MM2"

cd G4
eval `scramv1 runtime -sh`
for i in GV/gen*.root; do
	ln -s $i
done
./testMT.sh -t MM2 -s G4 -n 500 >& log_testMM2.log
echo "finished MM2 G4"

cd GV
eval `scramv1 runtime -sh`
./testMT.sh -t MM2 -s GVst -n 500 >& log_testMM2_st.log
echo "finished MM2 st"

