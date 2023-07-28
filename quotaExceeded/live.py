#!/usr/bin/python3

# copied from main

import apiclient
import requests
import subprocess
import shlex

KEY = "YOUR_API_KEY"
separator = '|SEPARATOR|'

def increaseRelation(src, dest):
    #print(src, relations)
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
        #print(sn)
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
        #print(parentId)
        commenterChannel = commentersChannels[parentId] if parentId in commentersChannels else parentId
        increaseRelation(commenterId, commenterChannel)

counter = 0
relations = {}
commentersChannels = {}
names = {}

youtube = apiclient.discovery.build('youtube', 'v3', developerKey=KEY)

def scrape(youtuberId, comments, video_id, token=None):
    global counter

    #youtube = apiclient.discovery.build('youtube', 'v3', developerKey=KEY)

    print(counter, len(relations))
    counter += 1

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
    with open(f'videos/{videoId}.txt', 'w') as f:
        for realKey in relations:
            intensity = relations[realKey]
            #print(f'realKey: {realKey} !');
            parts = realKey.split(separator)
            #print(parts);
            src, dest = parts
            #key, channel = parts
            keyName = names[src]
            channelName = names[dest]
            #f.write(f"{key} {channel} {intensity}\n")
            f.write(f"{keyName} {separator} {channelName} {separator} {intensity}\n")

#workVideo('gdZLi9oWNZg')#'_ZPpU7774DQ')

def getURL(url):
    response = requests.get(url)
    return response.text

def isFrench(youtuberId): # approved
    url = f'https://youtube.googleapis.com/youtube/v3/channels?part=snippet&id={youtuberId&key={KEY}'
    content = getURL(url)
    return '"country": "FR"' in content

def workChannel(youtuberId):
    #url = f'https://www.googleapis.com/youtube/v3/search?key={KEY}&channelId={youtuberId}&part=snippet,id&order=date&maxResults=50' # used to be 20
    #print('isFrench', isFrench(youtuberId))
    if isFrench(youtuberId):
        url = f'https://youtube.googleapis.com/youtube/v3/channels?part=contentDetails&id={youtuberId}&key={KEY}' # could merge request or make a cache with isFrench because can ask both for cost 1
        content = getURL(url)
        uploadsPlaylist = content.split('"uploads": "')[1].split('"')[0]
        #print(uploadsPlaylist)
        cmd = f'youtube-dl -j --flat-playlist {shlex.quote(\'https://www.youtube.com/playlist?list={uploadsPlaylist}\')} | jq -r .id'
        s = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
        idsStr = s.stdout.read().decode('utf-8')
        #print(subprocess_return)
        ids = idsStr.split("\\n")
        idsLen = len(ids)
        #print(idsLen)
        for idsIndex in range(idsLen):
            id = ids[idsIndex]
            workVideo(youtuberId, id)
            #print(id)

workChannel('UCWeg2Pkate69NFdBeuRFTAw')
#workChannel('UC3IZKseVpdzPSBaWxBxundA')

print('couldntWorkOn', couldntWorkOn)
