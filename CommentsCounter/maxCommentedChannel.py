cleanFile = 'C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\CommentsCounter\\clean.txt'

f = open(cleanFile, encoding = 'utf-8')
lines = f.readlines()
f.close()

maxCommentsNumber = 0
comments = {}

linesLen = len(lines)
for linesIndex in range(linesLen):
    line = lines[linesIndex]
    lineParts = line.split(' comments number: ')
    commentsNumber = int(lineParts[1].split(' videos number: ')[0].replace(' ', ''))
    youtuber = lineParts[0]
    comments[youtuber] = commentsNumber
    if commentsNumber > maxCommentsNumber:
        maxCommentsNumber = commentsNumber
        print(youtuber, commentsNumber)

comments = dict(sorted(comments.items(), key=lambda item: item[1]))
for youtuber in comments:
    print(youtuber, comments[youtuber])

