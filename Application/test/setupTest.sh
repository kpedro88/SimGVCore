#!/bin/bash

TESTNAME="$1"
TESTDIR=test${TESTNAME}

if [[ -d "$TESTDIR" ]]; then
	echo "$TESTDIR already exists"
	exit 1
fi

mkdir $TESTDIR

touch ${TESTDIR}/args.txt

echo $TESTDIR
