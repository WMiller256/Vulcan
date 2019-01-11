#!/bin/bash
for i in 2 3 4 5 6 7 8 9 1
do
	./opt_timeSampling.out explicit 86400e8 864000 $i 1e8-10
done
