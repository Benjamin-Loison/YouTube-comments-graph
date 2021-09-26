import os

path = "C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\"

os.chdir(path)

quick = False
vectorize = False

cleanFile = 'CommentsCounter\\clean.txt'

f = open(cleanFile, encoding = 'utf-8')
linesClean = f.readlines()
f.close()

linesCleanLen = len(linesClean)

# copied from checkLineByLineTry.py
def getTreatedComments(youtuberId):
    detailsFile = "F:\\YouTube\\CommentsGraph\\details\\" + youtuberId + ".txt"
    #print(detailsFile)
    if not os.path.isfile(detailsFile) :
        return 0
    fDetails = open(detailsFile, encoding = 'utf-8')
    linesDetails = []
    try:
        linesDetails = fDetails.readlines()
    except:
        fDetails.close()
        #return 0
        #print('a reading failed')
        return getTreatedComments(youtuberId)
    fDetails.close()
    linesDetailsLen = len(linesDetails)
    #print(linesDetailsLen)
    videosCommentsCounter = 0
    answeringCounter = 0
    for linesDetailsIndex in range(linesDetailsLen):
        line = linesDetails[linesDetailsIndex]
        #print('!' + str(linesDetailsIndex) + '!' + line + '!')
        lineParts = line.split()
        linePartsLen = len(lineParts)
        if linePartsLen >= 5:
            if lineParts[0][:2] == "Ug" and lineParts[1][:2] == "UC" and lineParts[2][:2] == "UC":
                if "." in lineParts[0]:
                    answeringCounter += 1
                    #print('answeringCounter', answeringCounter)
                elif lineParts[3] != youtuberId:
                    videosCommentsCounter += 1
                    #print('videosCommentsCounter', videosCommentsCounter)
    return videosCommentsCounter + (answeringCounter // 2)

def getComments(youtuber, length = -1):
    for linesCleanIndex in range(linesCleanLen):
        lineClean = linesClean[linesCleanIndex]
        lineCleanParts = lineClean.split(' comments number: ')
        lineCleanParts0 = lineCleanParts[0]
        if length == 0:
            print('definitively nothing for: ' + youtuber)
            return
        elif length > 0:
            lineCleanParts0 = lineCleanParts0[:length]
        if lineCleanParts0 == youtuber: # in clean should also add youtuberId otherwise because C++ read badly arabic characters we can't make the match for euronews
            #print('found for: ' + youtuber)
            return int(lineCleanParts[1].split(' videos number: ')[0].replace(' ', ''))
    #print('nothing found for: ' + youtuber)
    return getComments(youtuber[:-1], len(youtuber) - 1)

f = open('CPP\\logs.txt', encoding = 'utf-8') # \\0.txt
lines = f.readlines()
f.close()

pattern = ': working on '

treating = {}

maxComments = 0
maxCommentsYoutuber = ''
maxCommentsYoutuberId = ''

linesLen = len(lines)
for linesIndex in range(linesLen):
    line = lines[linesIndex]
    if pattern in line:
        lineParts = line.split(pattern)
        lineParts1 = lineParts[1]
        lineParts1Parts = lineParts1.split(' (') # UC
        lineParts1PartsLen = len(lineParts1Parts)
        youtuber = ' ('.join(lineParts1Parts[:lineParts1PartsLen - 1])#0] # this was problematic for euronews
        youtuberId = lineParts1Parts[lineParts1PartsLen - 1].split(')')[0] # 1
        lineParts = line.split(' - ')
        linePartsLen = len(lineParts)
        threadsIndex = int(lineParts[linePartsLen - 1])
        comments = getComments(youtuber)
        if threadsIndex in treating:
            treatedYoutuber, treatedYoutuberId, treatedComments = treating[threadsIndex]
            #treatedComments = getComments(treatedYoutuber)
            if treatedComments > maxComments:
                maxComments = treatedComments
                maxCommentsYoutuber = treatedYoutuber
                maxCommentsYoutuberId = treatedYoutuberId
                print(treatedYoutuber, maxComments)
        #print(youtuber, comments)
        treating[threadsIndex] = [youtuber, youtuberId, comments]

print()
print(maxCommentsYoutuber, maxCommentsYoutuberId, maxComments)

# should manage if a thread finished forever

print('\nstill working on:\n')
#for threadsIndex in treating:
#for threadsIndex in reversed(treating):
for threadsIndex in list(reversed(sorted(treating.keys()))):
    if threadsIndex != 0:
        continue
    comments = treating[threadsIndex][2]
    #if comments > 4 * (10 ** 6):
    #    continue
    if quick:
        if vectorize:
            print('\twasTreating.push_back("' + treating[threadsIndex][1] + '"); // ' + treating[threadsIndex][0])
        else:
            print(threadsIndex, treating[threadsIndex][0], comments)
    else:
        youtuberId = treating[threadsIndex][1]
        print(threadsIndex, end = ' ')
        print(treating[threadsIndex][0], end = ' ')
        treatedComments = getTreatedComments(youtuberId)
        print(treatedComments, end = ' ')
        print(comments, end = ' ')
        print(round(100 * treatedComments / comments, 2), end = ' ')
        print(youtuberId)
        #print(threadsIndex, treating[threadsIndex][0], treatedComments, comments, round(100 * treatedComments / comments, 2), youtuberId)
print()

# "it's funny" euronews seems to have a lot more comments than expected, i don't know why

##

threadsIndex = 3
comments = treating[threadsIndex][2]
treatedComments = getTreatedComments(treating[threadsIndex][1])
print(round(100 * treatedComments / comments, 2))

