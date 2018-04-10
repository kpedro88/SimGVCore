#!/bin/bash

NCPU=$(cat /proc/cpuinfo | grep processor | wc -l)

for ((th=1;th<=$NCPU;th++)); do
	# use other CPUs
	declare -A PIDS
	for ((busy=$((th+1));busy<=$NCPU;busy++)); do
		yes >& /dev/null &
		PIDS[$busy]=$!
	done
	# run main program
	cmsRun runGeantV_cfi.py nThreads=$th >& log_runSingleElectron50_th${th}.log
	# get results
	echo "threads: ${th}"
	grep "Total loop" log_runSingleElectron50_th${th}.log
	grep "MemoryCheck: event : VSIZE" log_runSingleElectron50_th${th}.log | awk ' { if($5>maxv){maxv=$5;}; if($8>maxr){maxr=$8;}} END {print " - peak VSIZE: ", maxv, "\n - peak RSS: ", maxr }'
	echo ""
	# kill busy processes
	for PID in ${PIDS[@]}; do
		kill $PID >& /dev/null
		wait $PID >& /dev/null
	done
done

