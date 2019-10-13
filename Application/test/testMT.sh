#!/bin/bash

TESTNAME=""
SIM=""
NEVENTS=0
NCPU=$(cat /proc/cpuinfo | grep processor | wc -l)
# check for hyperthreading
if grep -q " ht " /proc/cpuinfo; then
	NCPU=$((NCPU/2))
fi
ARGS="particle=electron mult=2 energy=50 year=2018 output=0"

while getopts "t:a:s:n:" opt; do
	case "$opt" in
		t) TESTNAME=$OPTARG
		;;
		a) ARGS="$OPTARG"
		;;
		s) SIM="$OPTARG"
		;;
		n) NEVENTS="$OPTARG"
		;;
	esac
done

if [ -z "$TESTNAME" ]; then
	echo "Must specify -t"
	exit 1
fi

if [ -z "$SIM" ]; then
	echo "Must specify -s"
	exit 1
fi

if [ "$NEVENTS" -eq 0 ]; then
	echo "Must specify -n"
	exit 1
fi

if [ -z "$ARGS" ]; then
	echo "Must have args"
	exit 1
fi

NOCHECK=""
if [ "$SIM" == GVst ]; then NOCHECK=true; fi
./setupTest.sh $TESTNAME $NOCHECK
SETUPEXIT=$?
if [[ $SETUPEXIT -ne 0 ]]; then
	exit $SETUPEXIT
fi

# check gen
GENNAME=$(python getGenName.py $ARGS maxEvents=$NEVENTS)
if ! [ -f ${GENNAME}.root ]; then
	cmsRun runGen.py $ARGS maxEvents=$NEVENTS
	CMSEXIT=$?
	if [[ $CMSEXIT -ne 0 ]]; then
		echo "Failure in gen ($GENNAME)"
		exit $CMSEXIT
	fi
fi

# check concatenated gen
TOTEVENTS=$(( NEVENTS*NCPU ))
TOTGENNAME=$(python getGenName.py $ARGS maxEvents=$TOTEVENTS)
if ! [ -f ${TOTGENNAME}.root ]; then
	cmsRun runCopy.py $ARGS maxEvents=$NEVENTS ncopy=$NCPU
	CMSEXIT=$?
	if [[ $CMSEXIT -ne 0 ]]; then
		echo "Failure in copy ($TOTGENNAME)"
		exit $CMSEXIT
	fi
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
	THISEVENTS=$(( th*NEVENTS ))
	TARGS="$ARGS maxEvents=$THISEVENTS maxEventsIn=$TOTEVENTS sim=$SIM threads=$th"
	echo "$TARGS" >> ${TESTDIR}/args.txt
	./runTest.sh -t $TESTNAME -a "$TARGS"
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
python analyzeTest.py -t $TESTNAME -i
