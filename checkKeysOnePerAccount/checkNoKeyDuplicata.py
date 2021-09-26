import os

path = "C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\checkKeysOnePerAccount\\"

os.chdir(path)

f = open("../CPP/keys.txt")
lines = f.readlines()
f.close()

keys = []

linesLen = len(lines)
for linesIndex in range(linesLen):
    line = lines[linesIndex]
    if line[-1] == "\n":
        line = line[:-1]
    if line in keys:
        print("key duplicated: " + line)
    else:
        keys += [line]

