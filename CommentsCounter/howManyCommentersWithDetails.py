import os

youtuberId = 'UCWeg2Pkate69NFdBeuRFTAw'

#path = "F:\\YouTube\\CommentsGraph\\details\\"
path = "C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\CommentsCounter\\checker\\fastData\\"

os.chdir(path)

f = open(youtuberId + '.txt', encoding = 'utf-8')
lines = f.readlines()
f.close()

commenters = {}#[] # may need to use dict to optimize even if "useless" - yes it's far more faster

linesLen = len(lines)
slice = linesLen // 100
for linesIndex in range(linesLen):
    if linesIndex % slice == 0:
        print(linesIndex // slice)
    line = lines[linesIndex]
    #if line[-1] == "\n":
    #    line = line[:-1]
    lineParts = line.split()
    linePartsLen = len(lineParts)
    if linePartsLen >= 5:
        if lineParts[0][:2] == "Ug" and lineParts[1][:2] == "UC" and lineParts[2][:2] == "UC":
            commenter = lineParts[1]
            if not commenter in commenters:
                #commenters += [commenter]
                commenters[commenter] = True

commentersLen = len(commenters)
print(commentersLen)

