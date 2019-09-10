#!/bin/bash

cleanup() {
	FOLDER=test${TESTNAME}/

	# cleanup gen, checking for symlinked files that were already moved
	if [ -L "$GENNAME" ]; then
		rm "$GENNAME"
	elif [ -f "$GENNAME" ]; then
		mv "$GENNAME" $FOLDER
	fi

	# cleanup sim (log, pp.gz, report, root)
	mv *$SIMNAME* $FOLDER
}

TESTNAME=""
ARGS=""
while getopts "t:a:" opt; do
	case "$opt" in
		t) TESTNAME=$OPTARG
		;;
		a) ARGS="$OPTARG"
		;;
	esac
done

if [ -z "$TESTNAME" ]; then
	echo "Must specify test name with -t"
	exit 1
fi

TESTDIR=test${TESTNAME}
GENNAME=$(python getGenName.py $ARGS)
SIMNAME=$(python getSimName.py $ARGS)

# check in test folder
if [ -f ${TESTDIR}/${GENNAME}.root ]; then
	ln -s ${TESTDIR}/${GENNAME}.root . >& /dev/null
fi
# still nothing?
if ! [ -f ${GENNAME}.root ]; then
	cmsRun runGen.py $ARGS
	CMSEXIT=$?
	if [[ $CMSEXIT -ne 0 ]]; then
		echo "Failure in gen ($GENNAME)"
		cleanup
		exit $CMSEXIT
	fi
fi

# now run igprof and generate report
IGNAME=igprof_${SIMNAME}
IGREP=igreport_${SIMNAME}.res

igprof -d -t cmsRun -pp -z -o ${IGNAME}.pp.gz cmsRun runSim.py $ARGS >& ${IGNAME}.log
IGEXIT=$?
if [[ $IGEXIT -ne 0 ]]; then
	echo "Failure in sim or igprof ($SIMNAME)"
	cleanup
	exit $IGEXIT
fi

igprof-analyse -d -v ${IGNAME}.pp.gz >& ${IGREP}
IGEXIT=$?
if [[ $IGEXIT -ne 0 ]]; then
	echo "Failure in igprof-analyse ($SIMNAME)"
	cleanup
	exit $IGEXIT
fi

cleanup
