import os

path = "C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\SocialBlade\\"

os.chdir(path)

f = open("top100fr.txt", encoding='utf-8')
lines = f.readlines()
f.close()

prefix = '<span style = "color:#555;">'

totalViews = 0
count = 0
linesLen = len(lines)
for linesIndex in range(linesLen):
    line = lines[linesIndex]
    if prefix in line:
        views = int(line.split(prefix)[1].split('</span>')[0].replace(',', ''))
        if count % 2 == 1:
            print(views)
            totalViews += views
        count += 1

print(totalViews)

