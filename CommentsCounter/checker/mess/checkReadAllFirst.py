import os, requests, json

path = "F:\\YouTube\\CommentsGraph\\details\\"

os.chdir(path)

# https://youtube.googleapis.com/youtube/v3/videos?part=statistics&id=bx613OLEn_Q&key=YOUR_API_KEY

youtuberId = 'UCpWaR3gNAQGsX48cIlQC0qw'#"UC-GI5LST5T3Gw93yZxjdFaw"#"UCLMKLU-ZuDQIsbjMvR3bbog"#"UCWeg2Pkate69NFdBeuRFTAw"#"UCgvqvBoSHB1ctlyyhoHrGwQ"#"UCWeg2Pkate69NFdBeuRFTAw"

f = open(youtuberId + ".txt", encoding = "utf-8")
lines = f.readlines()
f.close()

print('file read')

channelsCommentsCounter = 0
videosCommentsCounter = 0
answeringCounter = 0
unknownCounter = 0

lens = []

percent = linesLen // 1000

linesLen = len(lines)
for linesIndex in range(linesLen):
    if linesIndex % percent == 0:
        print(linesIndex // percent)
    line = lines[linesIndex]
    if line[-1] == "\n":
        line = line[:-1]
    lineParts = line.split()
    linePartsLen = len(lineParts)
    if linePartsLen >= 5:
        if lineParts[0][:2] == "Ug" and lineParts[1][:2] == "UC" and lineParts[2][:2] == "UC":
            #if lineParts[2] == "bx613OLEn_Q":
            if lineParts[2] != youtuberId:
                answeringCounter += 1
                #print(linesIndex, "answer")
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
                #print(linesIndex, "channel")
            else:
                videosCommentsCounter += 1
                #print(linesIndex, "video")

print(channelsCommentsCounter, videosCommentsCounter, answeringCounter, unknownCounter)

# Squeezie should be 14 579 287 # do video comments count also count answers ? yes it does # tested on https://www.youtube.com/watch?v=vSvLXYs05vs
# 0 14 254 652 1 569 276

# lost 2.23 % of comments, we have to understand what they are maybe some lost answers ? see Amixem

##

path = "C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\CPP\\channels\\"

os.chdir(path)

f = open(youtuberId + ".txt")
lines = f.readlines()
f.close()

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
    f = requests.get(url)
    content = f.text
    return content

commentsOnChannel = 0

# https://www.googleapis.com/youtube/v3/commentThreads?part=snippet,replies&channelId=UC-GI5LST5T3Gw93yZxjdFaw&key=YOUR_API_KEY returns no items

def scrap(pageToken = ''):
    url = 'https://www.googleapis.com/youtube/v3/commentThreads?part=snippet,replies&channelId=' + youtuberId + '&key=' + KEY
    content = getURL(url)
    data = json.loads(content)

scrap()
