import os, urllib.request, json, subprocess, time
from urllib.error import HTTPError
from datetime import datetime

path = 'C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\CommentsCounter\\'

# if speed is one request every 4 seconds we get all videos for a 100 000 videos channel in 4h (25 000 per hour)

os.chdir(path)

def log(s):
    now = datetime.now()
    print(now.strftime('%H:%M:%S'), s)

def getURL(url):
    res = ''
    try:
        res = urllib.request.urlopen(url).read()
    except HTTPError as e:
        res = e.read()
        #log(res)
    return res.decode('utf-8')

def exec(cmd):
    return subprocess.check_output(cmd).decode('utf-8')

BenjaminLoisonSecondary = 'UCt5USYpzzMCYhkirVQGHwKQ'
BenjaminLoison = 'UCF_UozwQBJY4WHZ7yilYkjA'
FranceInter = 'UCJldRgT_D7Am-ErRHQZ90uw'
France24 = 'UCCCPCZNChQdGa9EkATeye4g'
France24Arabic = 'UCdTyuXgmJkG_O8_75eqej-w'
euronewsInFrench = 'UCW2QcKZiU8aUGg4yxCIditg'
TheVoiceFR = 'UCQRELbX0H5FCokIFxOAsHFA'

youtuberId = TheVoiceFR#euronewsInFrench
videosIds = [] # could use a dict to improve speed but the main slowing factor is the internet request
errorsCount = 0

def retrieveVideosFromContent(content):
    global videosIds
    wantedPattern = '"videoId":"'
    content = content.replace('"videoId": "', wantedPattern).replace("'videoId': '", wantedPattern)
    #if '"videoId": "' in content:
    #    contentParts = content.split('"videoId": "')
    #else:
    contentParts = content.split(wantedPattern)
    for contentPart in contentParts:
        contentPartParts = contentPart.split('"')
        videoId = contentPartParts[0]
        videoIdLen = len(videoId)
        if not videoId in videosIds and videoIdLen == 11:
            videosIds += [videoId]

def scrap(token):
    global errorsCount, data # for debugging
    cmd = ['curl', '-s', 'https://www.youtube.com/youtubei/v1/browse?key=YOUR_API_KEY', '-H', 'Content-Type: application/json', '--data-raw', '{"context":{"client":{"clientName":"WEB","clientVersion":"CENSORED"}},"continuation":"' + token + '"}']
    cmd = cmd.replace('"', '\\"').replace("\'", '"')
    beginTime = time.time()
    content = exec(cmd)
    endTime = time.time()
    log(f'request took {endTime - beginTime} s')
    # not always 30 videos per answer (not talking about the last) - France24

    beginTime = time.time()
    retrieveVideosFromContent(content)
    endTime = time.time()
    log(f'retrieving videos took {endTime - beginTime} s')

    data = json.loads(content)
    if not 'onResponseReceivedActions' in data:
        log('no token found let\'s try again')
        errorsCount += 1
        return scrap(token)
    entry = data['onResponseReceivedActions'][0]['appendContinuationItemsAction']['continuationItems'][-1]
    if not 'continuationItemRenderer' in entry:
        return ''
    newToken = entry['continuationItemRenderer']['continuationEndpoint']['continuationCommand']['token']
    # in common: [-1]['continuationItemRenderer']['continuationEndpoint']['continuationCommand']['token']
    return newToken

url = f'https://www.youtube.com/channel/{youtuberId}/videos'
content = getURL(url)
content = content.split('var ytInitialData = ')[1].split(';</script>')[0]
dataFirst = json.loads(content)

retrieveVideosFromContent(content)

token = dataFirst['contents']['twoColumnBrowseResultsRenderer']['tabs'][1]['tabRenderer']['content']['sectionListRenderer']['contents'][0]['itemSectionRenderer']['contents'][0]['gridRenderer']['items'][-1]['continuationItemRenderer']['continuationEndpoint']['continuationCommand']['token']

while True:
    videosIdsLen = len(videosIds)
    log(f'{videosIdsLen} {token}')#, videosIds)
    if token == '':
        break
    newToken = scrap(token)
    token = newToken

# 2 videos difference ("late" in comparison with SocialBlade)
with open(f'videos/{youtuberId}.txt', 'w') as f:
    for videosIdsIndex in range(videosIdsLen):
        videoId = videosIds[videosIdsIndex]
        f.write(videoId)
        if videosIdsIndex != videosIdsLen - 1:
            f.write("\n")

# errorsCount: 42 for euronewsInFrench
