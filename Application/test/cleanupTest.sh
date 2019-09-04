#!/bin/bash

TESTNUM=0
while getopts "t:" opt; do
	case "$opt" in
		t) TESTNUM=$OPTARG
		;;
	esac
done

if [ $TESTNUM -eq 0 ]; then
	echo "Must specify test num with -t"
	exit 1
fi

while read ARGS; do
	# cleanup gen, checking for symlinked files that were already moved
	GENNAME=$(python getGenName.py $ARGS)
	if [[ -L "$GENNAME" ]]; then
		rm "$GENNAME"
	elif [[ -f "$GENNAME" ]]; then
		mv "$GENNAME" test${TESTNUM}/${GENNAME}
	fi

	# cleanup sim (log, pp.gz, report, root)
	SIMNAME=$(python getSimName.py $ARGS)
	mv *SIMNAME* test${TESTNUM}/
done < test${TESTNUM}/args.txt
