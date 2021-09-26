import os, urllib.request

path = "C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\keysChecker\\"

os.chdir(path)

f = open('message.txt')
lines = f.readlines()
f.close()

def getURL(url):
    return urllib.request.urlopen(url).read()

linesLen = len(lines)
for linesIndex in range(linesLen):
    line = lines[linesIndex]
    lineParts = line.split(',')
    key = lineParts[0]
    url = "https://youtube.googleapis.com/youtube/v3/channels?part=contentDetails&id=UCzYC9ss2P77Ry2LzIDL5Xsw&key=" + key
    try:
        content = getURL(url)
        if "UUzYC9ss2P77Ry2LzIDL5Xsw" in str(content):
            print(key)
        elif "quota" in str(content):
            print(key + " " + str(content))
    except:
        pass

