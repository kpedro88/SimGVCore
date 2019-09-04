#!/bin/bash

NCPU=$(cat /proc/cpuinfo | grep processor | wc -l)

TESTNUM=$(./setupTest.sh)

for ((th=1;th<=$NCPU;th++)); do
	# use other CPUs
	declare -A PIDS
	for ((busy=$((th+1));busy<=$NCPU;busy++)); do
		yes >& /dev/null &
		PIDS[$busy]=$!
	done

	# run test
	./runTest.sh -t $TESTNUM -a "particle=electron mult=2 energy=50 maxEvents=100 sim=GV year=2018 threads=$th"
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
analyzeTest.py -t $TESTNUM
