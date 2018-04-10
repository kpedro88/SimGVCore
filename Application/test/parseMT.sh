#!/bin/bash

echo "threads time CPU VSIZE RSS"
awk '{if($1=="threads:" && length(temp)>0){print temp; temp="";}; if(NF>1){temp = temp $(NF) " ";}} END {print temp}' $1

