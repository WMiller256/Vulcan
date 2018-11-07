#!/bin/bash

N="10 100 1000 10000 100000 1000000 10000000 100000000"
T="1 2 3 4 5 6"

for t in $T
do
	for n in $N
	do
		~/bin/bulirsch-stoer.exe explicit $n $t
		~/bin/bulirsch-stoer.exe single $n $t
		~/bin/bulirsch-stoer.exe omp $n $t
	done
done
	
