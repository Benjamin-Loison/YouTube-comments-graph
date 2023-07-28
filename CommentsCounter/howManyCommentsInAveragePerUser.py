import os

path = "C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\CPP\\channels\\"

os.chdir(path)

youtuber = 'UCpWaR3gNAQGsX48cIlQC0qw'

f = open(youtuber + '.txt')
lines = f.readlines()
f.close()

# here count twice when commenter comment the youtuber comment
#commenters = {}
comments = 0

linesLen = len(lines)
commenters = linesLen
for linesIndex in range(linesLen):
    line = lines[linesIndex]
    if line[-1] == '\n':
        line = line[:-1]
    lineParts = line.split()
    commenter = lineParts[0]
    respondingTo = lineParts[1]
    if respondingTo == youtuber:
    #    print('hey')
        comments += int(lineParts[2])
    #else:
    #    print(respondingTo)

print(comments / commenters)

##

#path = "F:\\YouTube\\CommentsGraph\\details\\"
path = "C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\CommentsCounter\\checker\\fastData\\"

os.chdir(path)

f = open(youtuber + '.txt', encoding = 'utf-8')
lines = f.readlines()
f.close()

# more precise (not taking into account twice times comments to the youtuber comment)

comments = 0
linesLen = len(lines) # not considering here multilines comments
for linesIndex in range(linesLen - 1):
    line = lines[linesIndex]
    nextLine = lines[linesIndex + 1]
    if line[-1] == '\n':
        line = line[:-1]
    if nextLine[-1] == '\n':
        nextLine = nextLine[:-1]
    lineParts = line.split()
    linePartsLen = len(lineParts)
    if linePartsLen >= 5 and line != nextLine and lineParts[2] == youtuber:#== nextLine: # and line != ''
        #print(linesIndex, line)
        comments += 1

print(comments / commenters)

##

for linesIndex in range(linesLen):
    line = lines[linesIndex]
    if line[-1] == '\n':
        line = line[:-1]
    """lineParts = line.split()
    linePartsLen = len(lineParts)
    if linePartsLen >= 5:
        if lineParts[0][:2] == "Ug" and lineParts[1][:2] == "UC" and lineParts[2][:2] == "UC":"""

