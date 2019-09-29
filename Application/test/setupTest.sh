#!/bin/bash

TESTNAME="$1"
NOCHECK="$2"
TESTDIR=test${TESTNAME}

if [ -z "$NOCHECK" ] && [[ -d "$TESTDIR" ]]; then
	echo "$TESTDIR already exists"
	exit 1
fi

mkdir -p $TESTDIR

touch ${TESTDIR}/args.txt

echo $TESTDIR
