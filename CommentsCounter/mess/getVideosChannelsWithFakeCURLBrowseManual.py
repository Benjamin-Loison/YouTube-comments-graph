import os, urllib.request, json, subprocess
from urllib.error import HTTPError

path = 'C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\CommentsCounter\\'

os.chdir(path)

def getURL(url):
    res = ""
    try:
        res = urllib.request.urlopen(url).read().decode('utf-8')
    except HTTPError as e:
        res = e.read()
    return res

def exec(cmd):
    return subprocess.check_output(cmd, shell = True)

FranceInter = 'UCJldRgT_D7Am-ErRHQZ90uw'
France24 = 'UCCCPCZNChQdGa9EkATeye4g'
France24Arabic = 'UCdTyuXgmJkG_O8_75eqej-w'
euronewsInFrench = 'UCW2QcKZiU8aUGg4yxCIditg'

youtuberId = FranceInter

url = 'https://www.youtube.com/channel/' + youtuberId + '/videos'
content = getURL(url)
content = content.split('var ytInitialData = ')[1].split(";</script>")[0]
data = json.loads(content)
#tokenPattern = '"token":"'
#contentParts = content.split(tokenPattern)
#token = contentParts[1].split('"')[0]
#print(token)

"""obj = data#['onResponseReceivedActions']
for key in obj.keys():
    print(key)"""

videosIds = []

def retrieveVideosFromContent(content):
    global videosIds
    if '"videoId": "' in content:
        contentParts = content.split('"videoId": "')
    else:
        contentParts = content.split('"videoId":"')
    contentPartsLen = len(contentParts)
    for contentPartsIndex in range(contentPartsLen):
        contentPart = contentParts[contentPartsIndex]
        contentPartParts = contentPart.split('"')
        videoId = contentPartParts[0]
        #print(videoId)
        videoIdLen = len(videoId)
        if not videoId in videosIds and videoIdLen == 11:
            videosIds += [videoId]

retrieveVideosFromContent(content)

videosIdsLen = len(videosIds)
print(videosIdsLen, videosIds)

entries = data['contents']['twoColumnBrowseResultsRenderer']['tabs']
"""entriesLen = len(entries)
for entriesIndex in range(entriesLen):
    entry = entries[entriesIndex]
    #print(entry)
    if 'continuationItemRenderer' in str(entry):
        print('hey', entriesIndex)
"""

entry = entries[1]['tabRenderer']['content']['sectionListRenderer']['contents'][0]['itemSectionRenderer']['contents'][0]['gridRenderer']['items'][-1]['continuationItemRenderer']['continuationEndpoint']['continuationCommand']['token'] # nous ce que l'on aime chez YouTube c'est les listes d'un élément
#print(str(entry))
token = entry

cmd = 'curl -s \'https://www.youtube.com/youtubei/v1/browse?key=YOUR_API_KEY\' -H \'Content-Type: application/json\' --data-raw \'{"context":{"client":{"clientName":"WEB","clientVersion":"CENSORED"}},"continuation":"' + token + '"}\''
cmd = cmd.replace('"', '\\"') # otherwise doesn't work on Windows if doesn't do both replacements
cmd = cmd.replace("\'", '"')
#print(cmd)
content = exec(cmd).decode('utf-8')

retrieveVideosFromContent(content)

data = json.loads(content)
entry = data['onResponseReceivedActions'][0]['appendContinuationItemsAction']['continuationItems'][-1]['continuationItemRenderer']['continuationEndpoint']['continuationCommand']['token']
token = entry

