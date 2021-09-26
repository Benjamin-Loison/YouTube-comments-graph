import os, requests, json

isWSL = False
def getPath(path):
    if not isWSL:
        return path
    return path.replace("\\", "/").replace("F:/", "/mnt/f/").replace("C:/", "/mnt/c/")

path = getPath("F:\\YouTube\\CommentsGraph\\details\\")
#path = "C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\CommentsCounter\\checker\\fastData\\"
#path = "C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\Usage\\Amixem\\fastData\\"

os.chdir(path)

# https://youtube.googleapis.com/youtube/v3/videos?part=statistics&id=bx613OLEn_Q&key=YOUR_API_KEY

youtubersIds = []
commonPath = 'C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\CommentsCounter\\'
f = open(getPath(commonPath + 'french100Ids.txt'))
lines = f.readlines()
f.close()
linesLen = len(lines)
for linesIndex in range(linesLen):
    line = lines[linesIndex]
    if line[-1] == "\n":
        line = line[:-1]
    youtubersIds += [line]

#youtuberId = 'UCWeg2Pkate69NFdBeuRFTAw'#'UCgvqvBoSHB1ctlyyhoHrGwQ'#'UCpWaR3gNAQGsX48cIlQC0qw'#"UC-GI5LST5T3Gw93yZxjdFaw"#"UCLMKLU-ZuDQIsbjMvR3bbog"#"UCWeg2Pkate69NFdBeuRFTAw"#"UCgvqvBoSHB1ctlyyhoHrGwQ"#"UCWeg2Pkate69NFdBeuRFTAw"

def checkYoutuber(youtuberId):
    filePath = youtuberId + ".txt"
    if not os.path.isfile(filePath):
        return 0
    f = open(filePath, encoding = "utf-8")
    lines = f.readlines()
    f.close()

    #print('file read')

    channelsCommentsCounter = 0
    videosCommentsCounter = 0
    answeringCounter = 0
    unknownCounter = 0

    lens = []

    linesLen = len(lines)
    percent = linesLen // 100
    for linesIndex in range(linesLen):
        #if linesIndex % percent == 0:
        #    print(linesIndex // percent)
        line = lines[linesIndex]
        if line[-1] == "\n":
            line = line[:-1]
        lineParts = line.split()
        linePartsLen = len(lineParts)
        if linePartsLen >= 5:
            if lineParts[0][:2] == "Ug" and lineParts[1][:2] == "UC" and lineParts[2][:2] == "UC":
                #if lineParts[2] == "bx613OLEn_Q":
                lineParts0Len = len(lineParts[0])
                if not lineParts0Len in lens:
                    lens += [lineParts0Len]
                    #print("!" + str(lineParts0Len) + "!" + line + "!")
                if "." in lineParts[0]:#lineParts[2] != youtuberId:
                    answeringCounter += 1
                    #print(linesIndex, "answer")
                else: # doesn't used to have this else
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

    answeringCounter //= 2 # because answers are written twice (one for youtuber and one for initial commenter)
    #print(channelsCommentsCounter, videosCommentsCounter, answeringCounter, unknownCounter)
    realCommentsNumber = answeringCounter + videosCommentsCounter
    #print("real comments number: " + str(realCommentsNumber))
    return realCommentsNumber

f = open(getPath(commonPath + 'log\\log.txt'))
lines = f.readlines()
f.close()
linesLen = len(lines)
expectedComments = []
for linesIndex in range(linesLen):
    line = lines[linesIndex]
    if line[-1] == "\n":
        line = line[:-1]
    expectedComments += [int(line)]

f = open(getPath(commonPath + 'names.txt'), encoding = 'utf-8')
lines = f.readlines()
f.close()
linesLen = len(lines)
names = []
for linesIndex in range(linesLen):
    line = lines[linesIndex]
    if line[-1] == "\n":
        line = line[:-1]
    names += [line]
# in my approach we assumed that commentsNumber declared and if found for real one then it is ok

expectedCommentsTotal = sum(expectedComments)
#print(expectedCommentsTotal) # 120 938 018
# i don't have arguments to justify the more than 100 % values - maybe people putting banning comments or disabling them and it's not just compensate new comments between retrieving and expectation but more
# furthermore then bit difference less than 100 % is because the expectation was run later than retrieving so there are more data now that we would expect
# even if not perfect current retrieving i'll work with that i think because it's globally ok

realCommentsNumberTotal = 0

#checkYoutuber('UCAdyNOE80FsFPYlFliyXfwQ')
youtubersIdsLen = len(youtubersIds)
for youtubersIdsIndex in range(youtubersIdsLen):#youtubersIdsLen - 1, -1, -1):
    youtuberId = youtubersIds[youtubersIdsIndex]
    expectedComment = expectedComments[youtubersIdsIndex]
    realCommentsNumber = checkYoutuber(youtuberId)
    realCommentsNumberTotal += realCommentsNumber
    percent = round(100 * realCommentsNumber / expectedComment, 2) if expectedComment != 0 else 100
    name = names[youtubersIdsIndex]
    print(name + ': ' + str(percent) + ': ' + str(realCommentsNumber) + ' / ' + str(expectedComment)) # used to be youtuberId instead of name

# Squeezie should be 14 579 287 # do video comments count also count answers ? yes it does # tested on https://www.youtube.com/watch?v=vSvLXYs05vs
# 0 14 254 652 1 569 276

# lost 2.23 % of comments, we have to understand what they are maybe some lost answers ? see Amixem

#print(realCommentsNumberTotal)
print(str(round(100 * realCommentsNumberTotal / expectedCommentsTotal, 2)) + ': ' + str(realCommentsNumberTotal) + ' / ' + str(expectedCommentsTotal)) # 93.49: 113069588 / 120938018

if isWSL:
    exit(42)

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
