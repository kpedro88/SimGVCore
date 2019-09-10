#!/bin/bash

TESTNAME=$1
SIM=$2

NCPU=$(cat /proc/cpuinfo | grep processor | wc -l)

./setupTest.sh $TESTNAME
SETUPEXIT=$?
if [[ $SETUPEXIT -ne 0 ]]; thn
	exit $SETUPEXIT
fi

TESTDIR=test${TESTNAME}
for ((th=1;th<=$NCPU;th++)); do
	# use other CPUs
	declare -A PIDS
	for ((busy=$((th+1));busy<=$NCPU;busy++)); do
		yes >& /dev/null &
		PIDS[$busy]=$!
	done

	# run test
	ARGS="particle=electron mult=2 energy=50 maxEvents=100 sim=$SIM year=2018 threads=$th"
	echo "$ARGS" >> ${TESTDIR}/args.txt
	./runTest.sh -t $TESTNAME -a "$ARGS"
	TESTEXIT=$?

	# kill busy processes
	for PID in ${PIDS[@]}; do
		kill $PID >& /dev/null
		wait $PID >& /dev/null
	done

	if [[ $TESTEXIT -ne 0 ]]; then
		exit $TESTEXIT
	fi
done

# do the analysis once the loop is finished
python analyzeTest.py -t $TESTNAME
