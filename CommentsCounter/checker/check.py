import os, requests, json

#path = "F:\\YouTube\\CommentsGraph\\details\\"
path = "C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\Usage\\Amixem\\fastData\\"

os.chdir(path)

# https://youtube.googleapis.com/youtube/v3/videos?part=statistics&id=bx613OLEn_Q&key=YOUR_API_KEY

youtuberId = 'UCgvqvBoSHB1ctlyyhoHrGwQ'#'UCpWaR3gNAQGsX48cIlQC0qw'#'UC-GI5LST5T3Gw93yZxjdFaw'#'UCLMKLU-ZuDQIsbjMvR3bbog'#'UCWeg2Pkate69NFdBeuRFTAw'#'UCgvqvBoSHB1ctlyyhoHrGwQ'#'UCWeg2Pkate69NFdBeuRFTAw'

channelsCommentsCounter = 0
videosCommentsCounter = 0
answeringCounter = 0
unknownCounter = 0

lens = []
linesIndex = 0

# this good method (for memory seems to take a very long time to run after a few millions lines) let's go for C++ (couldn't predict it ^^')

"""with open(youtuberId + '.txt', encoding = 'utf-8') as f:
    for line in f:
        #if line[-1] == "\n":
        #print(line)
        if linesIndex % 10000 == 0:
            print(linesIndex, line)
        linesIndex += 1"""

print(linesIndex)

with open(youtuberId + '.txt', encoding = 'utf-8') as fp:
    line = fp.readline()
    while line:
        print(linesIndex)
        line = fp.readline()
        linesIndex += 1
        if line[-1] == "\n":
            #print('hey')
            line = line[:-1]
        lineParts = line.split()
        linePartsLen = len(lineParts)
        if linePartsLen >= 5:
            if lineParts[0][:2] == 'Ug' and lineParts[1][:2] == 'UC' and lineParts[2][:2] == 'UC':
                #if lineParts[2] == 'bx613OLEn_Q':
                if lineParts[2] != youtuberId:
                    answeringCounter += 1
                    #print(linesIndex, 'answer')
                #if len(lineParts[2]) != 11:
                if lineParts[0] == lineParts[1]:
                    unknownCounter += 1
                """videoId = lineParts[4]
                videoIdLen = len(videoId)
                if not videoIdLen in lens:
                    lens += [videoIdLen]
                    print(videoId)"""
                if lineParts[3] == youtuberId:
                    channelsCommentsCounter += 1
                    #print(line)
                    #print(linesIndex, 'channel')
                else:
                    videosCommentsCounter += 1
                    #print(linesIndex, 'video')

print(channelsCommentsCounter, videosCommentsCounter, answeringCounter, unknownCounter)

# Squeezie should be 14 579 287 # do video comments count also count answers ? yes it does # tested on https://www.youtube.com/watch?v=vSvLXYs05vs
# 0 14 254 652 1 569 276

# lost 2.23 % of comments, we have to understand what they are maybe some lost answers ? see Amixem

##

path = "C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\CPP\\channels\\"

os.chdir(path)

with open(youtuberId + '.txt') as f:
    lines = f.readlines()

videosCommentsCounter = 0

linesLen = len(lines)
for linesIndex in range(linesLen):
    line = lines[linesIndex]
    if line[-1] == "\n":
        line = line[:-1]
    lineParts = line.split()
    linePartsLen = len(lineParts)
    if linePartsLen >= 3:
        if lineParts[1] == youtuberId:
            videosCommentsCounter += int(lineParts[2])

print(videosCommentsCounter)

##

KEY = 'KEY'
REAL_KEY = 'YOUR_API_KEY'

def getURL(url):
    url = url.replace(KEY, REAL_KEY)
    data = requests.get(url).json()
    return data

commentsOnChannel = 0

# https://www.googleapis.com/youtube/v3/commentThreads?part=snippet,replies&channelId=UC-GI5LST5T3Gw93yZxjdFaw&key=YOUR_API_KEY returns no items

def scrap(pageToken = ''):
    url = 'https://www.googleapis.com/youtube/v3/commentThreads?part=snippet,replies&channelId=' + youtuberId + '&key=' + KEY
    data = getURL(url)

scrap()
