import os, matplotlib.pyplot as plt

path = "C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\CPP\\"

os.chdir(path)

youtuberId = 'UCWeg2Pkate69NFdBeuRFTAw'

f = open('logs.txt', encoding = 'utf-8')
lines = f.readlines()
f.close()

def endsWith(haystack, pattern):
    return haystack[-len(pattern):] == pattern

X, Y = [], []
pattern0 = ': request took '
pattern1 = ' ms ! - 0'
timeIndex = 0
linesLen = len(lines)
for linesIndex in range(linesLen):
    line = lines[linesIndex]
    if line[-1] == "\n":
        line = line[:-1]
    if pattern0 in line and pattern1 in line: # endsWith(line, " - 0")
        #print(timeIndex)
        #print(line)
        lineParts = line.split(pattern0)
        linePartsLen = len(lineParts)
        if linePartsLen < 2:
            print(line)
        lineParts1 = lineParts[1]
        lineParts1Parts = lineParts1.split(pattern1)
        lineParts1PartsLen = len(lineParts1Parts)
        lineParts1Parts0 = lineParts1Parts[0]
        #print(line)
        try:
            time = int(lineParts1Parts0)
        except:
            continue
        if time > 2000:#100000:
            #print(line)
            continue
        X += [timeIndex]
        Y += [time]
        timeIndex += 1

plt.plot(X, Y)
plt.title("time (in ms) taken per request for Squeezie")
plt.show()

