#!/bin/ksh

echo "Running output sdiff comparison between lsdir and unix commands..."

sdiff <(ls -lR | grep '^d' | awk '{print $9}' | sort -d) <(lsdir | sort -d)

echo "Comparison complete."
