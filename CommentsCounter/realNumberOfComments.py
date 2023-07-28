#!/usr/bin/python3

# can only check youtube channels with up to 5 000 videos with such a key management system - well up to around 10 000 videos with new slicing system

import json
import requests
import subprocess
#import os
import shlex

#youtuberId = 'UCAdyNOE80FsFPYlFliyXfwQ'#'UCdTyuXgmJkG_O8_75eqej-w'#'UCpWaR3gNAQGsX48cIlQC0qw'#'UC-GI5LST5T3Gw93yZxjdFaw'#'UCLMKLU-ZuDQIsbjMvR3bbog'#'UCWeg2Pkate69NFdBeuRFTAw'
KEY = 'KEY'
keysIndex = 0
keys = []
#REAL_KEY = 'YOUR_API_KEY' # the automatic rotation keys system is required for "france 24 arabic"

#path =

#os.chdir(path)

with open('../CPP/keys.txt') as f:
    lines = f.readlines()
for line in lines:
    if line[-1] == '\n':
        line = line[:-1]
    keys += [line]
REAL_KEY = keys[keysIndex]
keysLen = len(keys)
#print(keysLen)

def getURL(url):
    global REAL_KEY, keysIndex, keysLen, keys
    originURL = url

    #url = url.replace(KEY, REAL_KEY)
    reversedURL = url[::-1]
    reversedKEY = ('&key=' + KEY)[::-1]
    reversedREAL_KEY = ('&key=' + REAL_KEY)[::-1]
    reversedURL = reversedURL.replace(reversedKEY, reversedREAL_KEY)
    url = reversedURL[::-1] # otherwise have a bug on this video :'( https://www.youtube.com/watch?v=6g6aUTQdKEY

    #print(url)
    data = requests.get(url).json()
    content = json.dumps(data, indent = 4)
    #print(content)
    if 'The request cannot be completed because you have exceeded your ' in content:
        if keysIndex >= keysLen: # warning looping continously if no available key
            keysIndex = 0
        else:
            keysIndex += 1
        log(f'new key index: {keysIndex}')
        #print(keysIndex)
        REAL_KEY = keys[keysIndex]

        return getURL(originURL)
    #print('error for: ' + url)
    return data

def exec(cmd):
    return subprocess.check_output(cmd, shell=True).decode('utf-8')

def log(s, endLog = '\n'):
    print(s, end  = endLog)
    with open('logLog.txt', 'a') as fLog:
        fLog.write(s + endLog)

#f = open('log.txt', 'w')

def checkYoutuber(youtuberId):
    data = getURL(f'https://youtube.googleapis.com/youtube/v3/channels?part=contentDetails&id={youtuberId}&key={KEY}')
    #print(data)
    uploadsPlaylist = data['items'][0]['contentDetails']['relatedPlaylists']['uploads']
    #print(uploadsPlaylist)
    cmd = f'youtube-dl -j --flat-playlist {shlex.quote(f\'https://www.youtube.com/playlist?list={uploadsPlaylist}\') | jq -r .id'
    idsStr = exec(cmd)
    slices = []
    if True:#idsStr != '': # otherwise problem of TheVoiceFR and should treat this case in a cleaner manner to have real comments number
        #print(idsStr)
        ids = idsStr.split("\\n")
        #print(ids)
        idsLen = len(ids)
        print(f'current idsLen: {idsLen}')
        if idsLen >= 19500 or idsStr == '':
            ids = []
            with open(f'videos/{youtuberId}.txt') as f:
                lines = f.readlines()
            idsLen = len(lines)
            for line in lines:
                if line[-1] == '\n':
                    line = line[:-1]
                ids += [line]
            print(f'new idsLen: {idsLen}')
        slice = []
        for id in ids:
            if len(slice) == 50:
                slices += [slice]
                slice = []
            slice += [id]

        if len(slice) > 0:
            slices += [slice]
    #else:
    #print('aïe')


    slicesLen = len(slices)
    log(str(slicesLen))

    totalComments = 0

    for slicesIndex in range(slicesLen): # 32 have disabled comments
        ids = slices[slicesIndex]
        idsLen = len(ids)
        #print(str(slicesIndex) + ' / ' + str(slicesLen) + ' - ' + str(totalComments))
        url = 'https://www.googleapis.com/youtube/v3/videos?part=statistics&id=' + ','.join(ids) + '&key=' + KEY
        data = getURL(url)
        commentsNumbers = []
        items = data['items']
        itemsLen = len(items)
        if idsLen != itemsLen:
            log(f'{slicesIndex} / {slicesLen}')
            log(f'{itemsLen} / {idsLen}')
            log(url)
            log(json.dumps(data, indent = 4))
        for item in items:
            statistics = item['statistics']
            if 'commentCount' in statistics:
                commentsNumber = statistics['commentCount']
            else:
                commentsNumber = 0
                id = item['id']
                #print('no commentCount: ' + id)
            #print('commentsNumber', commentsNumber)
            commentsNumbers += [int(commentsNumber)]
        for idsIndex in range(idsLen): # could multithread this part
            id = ids[idsIndex]
            #commentsNumber = commentsNumbers[idsIndex]
            # before checking if not disabled could check if there are comments first - done - but doing so remove the "disabled comments" message for video which have comments disabled on publication which the case on some of "real youtubers"
            #if commentsNumber >= 1:
            #print(id)
            #print(id + ' ' + str(idsIndex) + ' / ' + str(idsLen))

            commentsEnabled = False
            url = f'https://youtube.googleapis.com/youtube/v3/commentThreads?part=snippet&videoId={id}&key=' + KEY
            #print(url)
            data = getURL(url)
            content = json.dumps(data, indent = 4)
            if ' parameter has disabled comments.' in content:
                log('comments really disabled ' + id)
                continue
            if not 'items' in data:
                #print(f'no data for: {url} content: {content}')
                continue
            items = data['items']
            itemsLen = len(items)
            commentsEnabled = itemsLen > 0

            if not commentsEnabled:
                #print('comments disabled ' + id) # or no one on this video
                pass
            else:
                # in this counting system we still don't count comments on channel left
                #pass
                commentsNumber = commentsNumbers[idsIndex]
                totalComments += commentsNumber

    #print(totalComments)
    with open('log.txt', 'a') as f: # otherwise doesn't seem to save until program stop, it seems that in case of crash no problem but I want to be sure
        f.write(f"{totalComments}\n")

with open('clean.txt') as fClean:
    linesClean = fClean.readlines()
youtuberNames = []
for line in linesClean:
    if line[-1] == '\n':
        line = line[:-1]
    youtuberName = line.split(' comments number: ')[0]
    youtuberNames += [youtuberName]

"""with open('french100Ids.txt') as fIds:
    lines = fIds.readlines()
linesLen = len(lines)
for linesIndex in range(92, linesLen):#linesLen - 2, -1, -1):
    line = lines[linesIndex]
    if line[-1] == '\n':
        line = line[:-1]
    log(f'working on {youtuberNames[linesIndex]} {linesIndex} / {linesLen}', ' - ')
    checkYoutuber(line)"""

#f.close()

TheVoiceFR = 'UCQRELbX0H5FCokIFxOAsHFA'
FranceInter = 'UCJldRgT_D7Am-ErRHQZ90uw'
France24 = 'UCCCPCZNChQdGa9EkATeye4g'
France24Arabic = 'UCdTyuXgmJkG_O8_75eqej-w'
euronewsInFrench = 'UCW2QcKZiU8aUGg4yxCIditg'

checkYoutuber(TheVoiceFR)
#checkYoutuber(France24)
#checkYoutuber(France24Arabic)
#checkYoutuber(euronewsInFrench)
