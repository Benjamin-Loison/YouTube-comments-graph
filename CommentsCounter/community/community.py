import os

# doesn't manage post with at least a million of comments

path = "C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\CommentsCounter\\community\\"

os.chdir(path)

youtuber = 'JUL'#'Furious Jumper'#'Squeezie'

f = open(youtuber + '.txt', encoding = 'utf-8')
lines = f.readlines()
f.close()

pattern = 'size-default" aria-label="'

totalCommentsMinimum = 0
totalCommentsMaximum = 0

linesLen = len(lines)
for linesIndex in range(linesLen):
    line = lines[linesIndex]
    if pattern in line:
        line = line.replace('&nbsp;', '')#, ' ')
        lineParts = line.split(pattern)
        lineParts1 = lineParts[1]
        lineParts1Parts = lineParts1.split("commentaire") # used to be "commentaires" but a post of JUL has only a single comment :')
        lineParts1Parts0 = lineParts1Parts[0]
        lineParts1Parts0 = lineParts1Parts0.replace(',', '.')
        #print(lineParts1Parts0)
        factor = 1
        isThousand = lineParts1Parts0[-1] == 'k'
        if isThousand:
            factor = 1000
            lineParts1Parts0 = lineParts1Parts0[:-1]
        comments = int(float(lineParts1Parts0) * factor)
        totalCommentsMinimum += comments
        totalCommentsMaximum += comments
        if isThousand:
            totalCommentsMaximum += 1000

print(totalCommentsMinimum, totalCommentsMaximum)

