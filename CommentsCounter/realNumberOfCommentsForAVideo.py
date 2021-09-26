#!/usr/bin/python3

import json, urllib.request

videoId = 'jEEUmfW0qFQ'#'5g0j29G2w58'#'dnx4sj-Ul8I'#'ksxNFa59OAg'#'-df82Zx2sak'#'Iu9BZdtH7lY'#'5g0j29G2w58'#'bx613OLEn_Q'
KEY = 'KEY'
REAL_KEY = 'YOUR_API_KEY'
maxAnswers = 0

def getURL(url):
    url = url.replace(KEY, REAL_KEY)
    return urllib.request.urlopen(url).read()

def scrap(videoId, counter = 0, pageToken = '', callsIndex = 0):
    global maxAnswers
    print(callsIndex)
    url = 'https://youtube.googleapis.com/youtube/v3/commentThreads?part=snippet%2Creplies&maxResults=100&videoId=' + videoId + '&key=' + KEY + ('' if pageToken == '' else '&pageToken=' + pageToken)
    #print(url)
    content = getURL(url)
    data = json.loads(content)
    items = data['items']
    itemsLen = len(items)
    counter += itemsLen
    for itemsIndex in range(itemsLen):
        item = items[itemsIndex]
        if 'replies' in item:
            commentsLen = len(item['replies']['comments'])
            if commentsLen > maxAnswers:
                maxAnswers = commentsLen
                print("maxAnswers", maxAnswers)
            #print("commentsLen:", commentsLen)
            counter += commentsLen
    if 'nextPageToken' in data:
        nextPageToken = data['nextPageToken']
        if nextPageToken != pageToken:
            counter = scrap(videoId, counter, nextPageToken, callsIndex + 1)
    return counter

print(scrap(videoId))
print(maxAnswers)
