import os

path = "C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\CommentsCounter\\"

os.chdir(path)

f = open('logs/logs050821.txt', encoding = 'utf-8')
#f = open('logs050821.txt')
lines = f.readlines()
f.close()

commentsDict = {}
videosDict = {}
totalComments = 0
totalVideos = 0
maxComments = 0
maxVideos = 0
commentsPattern = " comments number: "
videosPattern = " videos number: "
linesLen = len(lines)
for linesIndex in range(linesLen):
    line = lines[linesIndex]
    youtuber = line.split(commentsPattern)[0]
    commentsNumberStr = line.split(commentsPattern)[1].split(" videos")[0].replace(' ', '')
    videosNumberStr = line.split(videosPattern)[1].replace(' ', '')
    commentsNumber = int(commentsNumberStr)
    videosNumber = int(videosNumberStr)
    commentsDict[youtuber] = commentsNumber
    videosDict[youtuber] = videosNumber
    #print(commentsNumber)
    totalComments += commentsNumber
    totalVideos += videosNumber
    if commentsNumber > maxComments:
        maxComments = commentsNumber
    if videosNumber > maxVideos:
        maxVideos = videosNumber

# does this count also replies ?
print(totalComments, maxComments) # 118 779 450, 14 579 287 Squeezie
print(totalVideos, maxVideos) # 215 309, 20 001

#commentsDict = dict(sorted(commentsDict.items(), key=lambda item: item[1]))
videosDict = dict(sorted(videosDict.items(), key=lambda item: item[1]))

for youtuber in commentsDict:
    commentsNumber = commentsDict[youtuber]
    print(commentsNumber, youtuber)

"""for youtuber in videosDict:
    videosNumber = videosDict[youtuber]
    print(videosNumber, youtuber)"""

