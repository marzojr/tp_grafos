#!/bin/bash

if [[ ! -f "$1" ]]; then
	echo "Missing file name!"
	exit 1
fi

rm -rf plots
mkdir plots

grep -A 1 'Dijkstra' "$1" | egrep -v '(====|--)' | column -s " 	=," -t | awk -f format-data.awk | sort -k +1n | awk -f process-data.awk | tee plots/dijks.data &> /dev/null
grep -A 1 'A\*' "$1" | egrep -v '(====|--)' | column -s " 	=," -t | awk -f format-data.awk | sort -k +1n | awk -f process-data.awk | tee plots/astar.data &> /dev/null
grep -A 1 'JPS' "$1" | egrep -v '(====|--)' | column -s " 	=," -t | awk -f format-data.awk | sort -k +1n | awk -f process-data.awk | tee plots/jumps.data &> /dev/null

gnuplot *.gp
