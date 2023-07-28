import os

path = "C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\CommentsCounter\\videos\\"

os.chdir(path)

f = open('franceInterIds.txt')
lines = f.readlines()
f.close()

linesLen = len(lines)
ids = []
for linesIndex in range(linesLen):
    line = lines[linesIndex]
    if line[-1] == '\n':
        line = line[:-1]
    if not line in ids:
        ids += [line]
    else:
        print('wut')

f = open('UCJldRgT_D7Am-ErRHQZ90uw.txt', encoding = 'utf-8')
lines = f.readlines()
f.close()

linesLen = len(lines)
for linesIndex in range(linesLen):
    line = lines[linesIndex]
    if line[-1] == '\n':
        line = line[:-1]
    lineParts = line.split()
    id = ids[linesIndex]
    videoId = lineParts[0]
    if videoId != id:
        print('differs at ' + str(linesIndex))
        break
    else:
        print('ok ' + str(linesIndex))

