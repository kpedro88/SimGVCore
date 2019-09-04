#!/bin/bash

TESTNUM=0
ARGS=""
while getopts "t:a:" opt; do
	case "$opt" in
		t) TESTNUM=$OPTARG
		;;
		a) ARGS="$OPTARG"
		;;
	esac
done

if [ $TESTNUM -eq 0 ]; then
	echo "Must specify test num with -t"
	exit 1
fi

GENNAME=$(python getGenName.py $ARGS)

# check in test folder
if [ -f test${TESTNUM}/${GENNAME}.root ]; then
	ln -s test${TESTNUM}/${GENNAME}.root . >& /dev/null
fi
# still nothing?
if ! [ -f ${GENNAME}.root ]; then
	cmsRun runGen.py $ARGS
	CMSEXIT=$?
	if [[ $CMSEXIT -ne 0 ]]; then
		echo "Failure in gen ($GENNAME)"
		exit $CMSEXIT
	fi
fi

# now run igprof and generate report
SIMNAME=$(python getSimName.py $ARGS)
IGNAME=igprof_${SIMNAME}
IGREP=igreport_${SIMNAME}.res

igprof -d -t cmsRun -pp -z -o ${IGNAME}.pp.gz cmsRun $ARGS >& ${IGNAME}.log
IGEXIT=$?
if [[ $IGEXIT -ne 0 ]]; then
	echo "Failure in sim or igprof ($SIMNAME)"
	exit $IGEXIT
fi

igprof-analyse -d -v ${IGNAME}.pp.gz >& ${IGREP}
IGEXIT=$?
if [[ $IGEXIT -ne 0 ]]; then
	echo "Failure in igprof-analyse ($SIMNAME)"
	exit $IGEXIT
fi

