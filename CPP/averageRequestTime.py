import os

path = "C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\CPP\\"

#: request took N ms ! -

os.chdir(path)

f = open('logs.txt')
lines = f.readlines()
f.close()

totalTime, numberTime = 0, 0

linesLen = len(lines)
for linesIndex in range(linesLen):
    line = lines[linesIndex]
    if line[-1] == "\n":
        line = line[:-1]
    if ": request took " in line:
        lineParts = line.split(": request took ") # we also consider non api requests here
        linePartsLen = len(lineParts)
        if linePartsLen >= 2:
            linePart = lineParts[1]
            linePartParts = linePart.split(" ms ! - ")
            linePartPartsLen = len(linePartParts)
            if linePartPartsLen >= 2:
                linePartPart = linePartParts[0]
                time = int(linePartPart)
                #print(time)
                totalTime += time
                numberTime += 1

print(numberTime, totalTime / numberTime) # displaying just totalTime isn't really accurate here

