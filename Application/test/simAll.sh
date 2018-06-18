#!/bin/bash

SIMTYPE=$1

# single threaded tests
for year in 2018 2023; do
	for particle in electron photon; do
		for mult in 1 2 5 10 25 50 100; do
			for pt in 10 25 50 100; do
				cmsRun runSim.py particle=${particle} mult=${mult} pt=${pt} maxEvents=1000 year=${year} sim=${SIMTYPE} >& log_${SIMTYPE}_year${year}_${particle}_mult${mult}_pt${pt}_n1000.log
			done
		done
	done
done
