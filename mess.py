#!/usr/bin/python3

# copied from main

import apiclient

KEY = "YOUR_API_KEY"
separator = '|SEPARATOR|'

def increaseRelation(src, dest):
    #print(src, relations)
    if src in relations:
        relations[src + separator + dest] = [dest, relations[src][1] + 1]
    else:
        relations[src] = [dest, 1]

youtuberId = 'UCWeg2Pkate69NFdBeuRFTAw'

def build_comment(sn, id = ''):
    commenterId = sn['authorChannelId']['value']
    name = sn['authorDisplayName']
    names[commenterId] = name
    increaseRelation(commenterId, youtuberId)
    if id != '':
        commentersChannels[id] = commenterId
    else:
        parentId = sn['parentId']
        #print(parentId)
        commenterChannel = commentersChannels[parentId]
        increaseRelation(commenterId, commenterChannel)

counter = 0
relations = {}
commentersChannels = {}
names = {}

youtube = apiclient.discovery.build('youtube', 'v3', developerKey=KEY)

def scrape(comments, video_id, token=None):
    global counter

    #youtube = apiclient.discovery.build('youtube', 'v3', developerKey=KEY)

    print(counter)
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
        build_comment(topLevelComment['snippet'], topLevelComment['id'])
        if 'replies' in item.keys():
            for reply in item['replies']['comments']:
                build_comment(reply['snippet'])

    #return
    if 'nextPageToken' in results:
        scrape(comments, video_id, results['nextPageToken'])

def workVideo(videoId):
    scrape([], videoId)
    #global names
    names[youtuberId] = 'SQUEEZIE';
    separator = '|'
    f = open('videos/' + videoId + '.txt', 'w')
    for key in relations:
        channelIntensity = relations[key]
        channel, intensity = channelIntensity
        keyName = names[key]
        channelName = names[channel]
        #f.write(key + ' ' + channel + ' ' + str(intensity) + "\n")
        f.write(keyName + separator + channelName + separator + str(intensity) + "\n")
    f.close()

workVideo('_ZPpU7774DQ')
