#!/bin/bash

for particle in electron photon; do
	for mult in 1 2 5 10 25 50 100; do
		for pt in 10 25 50 100; do
			cmsRun runGen.py particle=${particle} mult=${mult} pt=${pt} maxEvents=1000 >& log_${particle}_mult${mult}_pt${pt}_n1000.log
		done
	done
done
