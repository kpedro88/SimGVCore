#!/bin/bash

for i in log_Geant*.log; do
	echo $i
	grep "Total loop" $i
	grep "MemoryCheck: event : VSIZE" $i | awk ' { if($5>maxv){maxv=$5;}; if($8>maxr){maxr=$8;}} END {print " - peak VSIZE: ", maxv, "\n - peak RSS: ", maxr }'
done
