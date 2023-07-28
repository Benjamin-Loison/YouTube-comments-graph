import os

path = "C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\CPP\\"

os.chdir(path)

f = open('logs.txt')
lines = f.readlines()
f.close()

def endsWith(subject, pattern):
    return subject[-len(pattern):] == pattern

linesLen = len(lines)
for linesIndex in range(linesLen):
    line = lines[linesIndex]
    if line[-1] == '\n':
        line = line[:-1]
    if endsWith(line, " - 0"):
        print(line)

