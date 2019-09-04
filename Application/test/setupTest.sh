#!/bin/bash

# automatically set up a new directory for this test
TESTNUM=1
while [[ -d test${TESTNUM} ]]; do
  TESTNUM=$((TESTNUM+1))
done
mkdir test${TESTNUM}

touch test${TESTNUM}/args.txt

echo $TESTNUM
