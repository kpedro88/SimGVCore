#!/bin/bash

SIMTYPE=$1

if [ -z "$SIMTYPE" ]; then
	echo "Must specify sim type"
	exit 1
fi

# single threaded tests
for year in 2018 2023; do
	for particle in electron photon; do
#		for mult in 1 2 5 10 25 50 100; do
		for mult in 1 2 5 10 25; do
			for pt in 10 25 50 100; do
				LOGNAME=log_${SIMTYPE}_year${year}_${particle}_mult${mult}_pt${pt}_n100.log
				if [ -e $LOGNAME ]; then continue; fi
#				echo $LOGNAME
				cmsRun runSim.py particle=${particle} mult=${mult} pt=${pt} maxEvents=100 maxEventsIn=1000 year=${year} sim=${SIMTYPE} >& log_${SIMTYPE}_year${year}_${particle}_mult${mult}_pt${pt}_n100.log
			done
		done
	done
done
