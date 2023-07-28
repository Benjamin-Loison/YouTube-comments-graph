#!/usr/bin/python3

# copied from main

import apiclient, requests, subprocess, os
#from datetime import date
from datetime import datetime

#could use a set of keys
#KEY = "YOUR_API_KEY"
#KEY = "YOUR_API_KEY"
KEY = 'KEY'#"YOUR_API_KEY"
separator = '|SEPARATOR|'

# every key was checked using https://youtube.googleapis.com/youtube/v3/channels?id=UCWeg2Pkate69NFdBeuRFTAw&key=KEY

# YOUR_EMAIL, YOUR_EMAIL, YOUR_EMAIL, YOUR_EMAIL, YOUR_EMAIL, YOUR_EMAIL, YOUR_EMAIL, YOUR_EMAIL
KEYS = ['YOUR_API_KEY', 'YOUR_API_KEY', 'YOUR_API_KEY', 'YOUR_API_KEY', 'YOUR_API_KEY', 'YOUR_API_KEY', 'YOUR_API_KEY', 'YOUR_API_KEY']
keyIndex = 0
REAL_KEY = KEYS[0]

costConsumed = 0

def changeKey():
    global keyIndex, REAL_KEY, youtube
    if keyIndex >= len(KEYS):
        log('all keys exhausted')
        exit(42)
    log(f'key {keyIndex} consumed increasing!')
    keyIndex += 1
    REAL_KEY = KEYS[keyIndex]
    youtube = apiclient.discovery.build('youtube', 'v3', developerKey=REAL_KEY)
    log('REAL KEY: ' + REAL_KEY)

logsFile = 'logs.txt'
#fLogs = open(logsFile, 'w')

if os.path.isfile(logsFile):
    os.remove(logsFile)

def log(s):
    #now = str(date.today())
    now = datetime.now().strftime('%d/%m/%Y %H:%M:%S.%f')
    s = now + ': ' + s
    print(s)
    with open(logsFile, 'a') as fLogs:
        fLogs.write(s + '\n')

def debug(s):
    if True:
        log(s)

def increaseRelation(src, dest):
    #log(src + ' -> ' relations)
    key = src + separator + dest
    if key in relations:
        relations[key] = relations[key] + 1
    else:
        relations[key] = 1

#youtuberId = 'UC3IZKseVpdzPSBaWxBxundA'
#youtuberId = 'UCWeg2Pkate69NFdBeuRFTAw'
couldntWorkOn = 0

def build_comment(youtuberId, sn, id = ''):
    global couldntWorkOn
    if not 'authorChannelId' in sn or not 'value' in sn['authorChannelId']:
        #log(sn)
        couldntWorkOn += 1
        return
    commenterId = sn['authorChannelId']['value']
    name = sn['authorDisplayName']
    names[commenterId] = name
    increaseRelation(commenterId, youtuberId)
    if id != '':
        commentersChannels[id] = commenterId
    else:
        parentId = sn['parentId']
        #log(parentId)
        commenterChannel = commentersChannels[parentId] if parentId in commentersChannels else parentId
        increaseRelation(commenterId, commenterChannel)

counter = 0
relations = {}
commentersChannels = {}
names = {}

youtube = apiclient.discovery.build('youtube', 'v3', developerKey=REAL_KEY)

def scrape(youtuberId, comments, video_id, token=None):
    global counter, costConsumed

    #youtube = apiclient.discovery.build('youtube', 'v3', developerKey=KEY)

    #log(str(counter) + ' / ' + str(len(relations)))
    counter += 1

    debug('before comment get')
    try:
        if token:
            results = youtube.commentThreads().list(
                part="snippet,replies",
                videoId=video_id,
                pageToken=token,
                textFormat="plainText",
                maxResults=100
            ).execute()
        else:
            results = youtube.commentThreads().list(
                part="snippet,replies",
                videoId=video_id,
                textFormat="plainText",
                maxResults=100
            ).execute()
    except:
        changeKey()
        scrape(youtuberId, comments, video_id, token)
        return
    debug('after comment get')
    costConsumed += 1

    for item in results['items']:
        topLevelComment = item['snippet']['topLevelComment']
        build_comment(youtuberId, topLevelComment['snippet'], topLevelComment['id'])
        if 'replies' in item.keys():
            for reply in item['replies']['comments']:
                build_comment(youtuberId, reply['snippet'])

    #return
    if 'nextPageToken' in results:
        scrape(youtuberId, comments, video_id, results['nextPageToken'])

def workVideo(youtuberId, videoId):
    scrape(youtuberId, [], videoId)
    #global names
    names[youtuberId] = 'SQUEEZIE'; # TODO: auto
    #names[youtuberId] = 'HYBE LABELS';
    #separator = '|'
    with open('videos/' + videoId + '.txt', 'w') as f:
        for realKey in relations:
            intensity = relations[realKey]
            #log('realKey: ' + realKey + ' !');
            parts = realKey.split(separator)
            #log(parts);
            src, dest = parts
            #key, channel = parts
            keyName = names[src]
            channelName = names[dest]
            #f.write(f"{key} {channel} {intensity}\n")
            f.write(f"{keyName} {separator} {channelName} {separator} {intensity}\n")

#workVideo('gdZLi9oWNZg')#'_ZPpU7774DQ')

def getItem(url):
    originalURL = url
    log('before: ' + url)
    url = url.replace('KEY', REAL_KEY)
    log('asked: ' + url)
    error = False
    try:
        response = requests.get(url)
    except:
        error = True
    if not error:
        error = response.status_code != 200
    if error:
        changeKey()
        return getItem(originalURL)
    return response.json()['items'][0]

def isFrench(youtuberId): # approved
    global costConsumed
    costConsumed += 1
    url = f'https://youtube.googleapis.com/youtube/v3/channels?part=snippet&id={youtuberId}&key={KEY}'
    item = getItem(url)
    return item['snippet']['country']== 'FR'

def workChannel(youtuberId):
    #url = f'https://www.googleapis.com/youtube/v3/search?key={KEY}&channelId={youtuberId}&part=snippet,id&order=date&maxResults=50' # used to be 20
    #log(f'isFrench {isFrench(youtuberId)})
    global costConsumed
    if isFrench(youtuberId):
        costConsumed += 1
        url = f'https://youtube.googleapis.com/youtube/v3/channels?part=contentDetails&id={youtuberId}&key={KEY}' # could merge request or make a cache with isFrench because can ask both for cost 1
        item = getItem(url)
        uploadsPlaylist = item['contentDetails']['relatedPlaylists']['uploads']
        #log(uploadsPlaylist)
        cmd = f"youtube-dl -j --flat-playlist \"https://www.youtube.com/playlist?list={uploadsPlaylist}\" | jq -r '.id'"
        s = subprocess.Popen(cmd, stdout=subprocess.PIPE)
        idsStr = str(s.stdout.read())[2:-3]
        #log(subprocess_return)
        ids = idsStr.split("\\n")
        idsLen = len(ids)
        #log(idsLen)
        for idsIndex in range(idsLen):
            id = ids[idsIndex]
            log(f'{idsIndex} \ {idsLen} {len(relations)} {costConsumed}')
            workVideo(youtuberId, id)
            #log(id)

workChannel('UCWeg2Pkate69NFdBeuRFTAw')
#workChannel('UC3IZKseVpdzPSBaWxBxundA')

log('couldntWorkOn', couldntWorkOn)

# TODO: recode with C++ with threads and add channels save or live discovery
#fLogs.close()
