#!/bin/bash
for j in {1..10};
do
	for i in {1..9};
	do
		./opt_timeSampling.exe explicit 86400e8 864000 $i 1e8-10.beast
	done
done
