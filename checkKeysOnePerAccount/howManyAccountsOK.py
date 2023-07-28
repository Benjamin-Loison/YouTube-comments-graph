import os, urllib.request, subprocess
from urllib.error import HTTPError

# after PST midnight we goes from 59 to 60 free keys maybe the thirteen remaining have 0 quota ?
# 07/09/21
# furthermore the 0 quota bug seems also to happen when you respect the ToS by just making a single project

path = "C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\checkKeysOnePerAccount\\"

os.chdir(path)

commonPath = '..\\CPP\\'
keysFile = commonPath + 'keys.txt'
accountsFile = commonPath + 'accounts.txt'

f = open(keysFile)
keysLines = f.readlines()
f.close()

f = open(accountsFile)
accounts = f.readlines()
f.close()

def removeFromFile(file, line):
    pass
    # TODO:

accountsLen = len(accounts)
for accountsIndex in range(accountsLen):
    account = accounts[accountsIndex]
    if account[-1] == '\n':
        account = account[:-1]
    accounts[accountsIndex] = account

def getURL(url):
    res = ""
    try:
        res = urllib.request.urlopen(url).read()
    except HTTPError as e:
        res = e.read()
    return res

removed = []
free, exhausted = 0, 0
keysLinesLen = len(keysLines)
for linesIndex in range(keysLinesLen):
    line = keysLines[linesIndex]
    #if accounts[linesIndex] != "YOUR_EMAIL":
    #    continue
    if line[-1] == '\n':
        line = line[:-1]
    #if line != "YOUR_API_KEY":
    #    continue
    url = "https://youtube.googleapis.com/youtube/v3/channels?part=contentDetails&id=UCzYC9ss2P77Ry2LzIDL5Xsw&key=" + line
    print(line, end = "")
    #try:
    content = getURL(url)
    if "UUzYC9ss2P77Ry2LzIDL5Xsw" in str(content):
        print(" X")
        free += 1
    elif "YouTube Data API v3 has not been used in project" in str(content):
        print(" @")
    elif "quota" in str(content):
        print(" $")
        exhausted += 1
    elif "suspended" in str(content):
        #print(" SUSPENDED - " + line + " " + accounts[linesIndex])
        removed += [[linesIndex, line, accounts[linesIndex]]]
        print(" " + accounts[linesIndex])
    else:
        print(" ERROR " + str(content))
    #except:
    #    print(" ERROR: " + line)

removedLen = len(removed)
print(free, exhausted, free + exhausted, keysLinesLen, removedLen)

# it seems that after using this need to use dos2unix to work correctly in my huge cpp program weird
def write(fileName, lines):
    linesLen = len(lines)
    f = open(fileName, 'w')
    for linesIndex in range(linesLen):
        line = lines[linesIndex]
        if line[-1] == '\n':
            line = line[:-1]
        f.write(line)
        if linesIndex < linesLen - 1:
            f.write('\n')
    f.close()
    callProcess = subprocess.Popen(['dos2unix', fileName]) # also working on Windows

if removedLen > 0:
    if True:
        for removedIndex in range(removedLen):
            removedEl = removed[removedIndex]
            print(removedEl)
            linesIndex, key, account = removedEl
    f = open(commonPath + 'removedBecauseSuspended.txt', 'a')
    for removedIndex in range(removedLen - 1, -1, -1):
        removedEl = removed[removedIndex]
        linesIndex, key, account = removedEl
        keysLines = keysLines[:linesIndex] + keysLines[linesIndex + 1:]
        accounts = accounts[:linesIndex] + accounts[linesIndex + 1:]
        f.write('\n' + key + ' ' + account)
    f.close()
    write(keysFile, keysLines)
    write(accountsFile, accounts)

## unique lines

def read(fileName):
    f = open(fileName)
    lines = f.readlines()
    f.close()
    return lines

def numberOfUniqueLines(fileName):
    return len(set(read(fileName)))

#print(numberOfUniqueLines(keysFile))
print(numberOfUniqueLines(accountsFile))
print(numberOfUniqueLines(commonPath + 'mess\\280821\\accounts.txt'))

# still one account from every email which is the most important because otherwise we loose definitively some access
