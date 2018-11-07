#!/usr/bin/python
import matplotlib.pyplot as plt

with open('data.txt', 'r') as fp:
	lines = fp.readlines();

for kk in range(1):
	for ii in range(len(lines)):
		line = lines[ii].split(" ");
		print line



