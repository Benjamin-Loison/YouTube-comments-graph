#!/usr/bin/python

f = open('logs.txt')
lines = f.readlines()
f.close()

sumComments = 0
linesLen = len(lines)
for linesIndex in range(2, linesLen - 1):
    line = lines[linesIndex]
    lineParts = line.split()
    sumComments += int(lineParts[3])

print(sumComments)
