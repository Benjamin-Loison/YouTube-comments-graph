import os, requests, json

path = "C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\CommentsCounter\\"

os.chdir(path)

FranceInter = 'UCJldRgT_D7Am-ErRHQZ90uw'
France24 = 'UCCCPCZNChQdGa9EkATeye4g'
France24Arabic = 'UCdTyuXgmJkG_O8_75eqej-w'
euronewsInFrench = 'UCW2QcKZiU8aUGg4yxCIditg'

youtuberId = FranceInter

KEY = 'KEY'
keysIndex = 0
keys = []
maxResults = 50

f = open('..\\CPP\\keys.txt')
lines = f.readlines()
f.close()
linesLen = len(lines)
for linesIndex in range(linesLen):
    line = lines[linesIndex]
    if line[-1] == "\n":
        line = line[:-1]
    keys += [line]
REAL_KEY = keys[keysIndex]
keysLen = len(keys)

def getURL(url):
    global REAL_KEY, keysIndex, keysLen, keys
    originURL = url

    url = url.replace('&key=' + KEY, '&key=' + REAL_KEY)

    print(url)
    f = requests.get(url)
    content = f.text
    if "The request cannot be completed because you have exceeded your " in content:
        if keysIndex >= keysLen:
            keysIndex = 0
        else:
            keysIndex += 1
        print("new key index: " + str(keysIndex))
        REAL_KEY = keys[keysIndex]

        return getURL(originURL)
    return content

videosIds, playlistsIds, videosTitles = [], [], []
callsIndex = 0
totalResults = 0

def scrap(pageToken = '', publishedBefore = ''):
    global callsIndex, videosIds, playlistsIds, totalResults, videosTitles
    if callsIndex == 30:
        return
    url = 'https://www.googleapis.com/youtube/v3/search?part=snippet&channelId=' + youtuberId + '&maxResults=' + str(maxResults) + '&type=video&order=date&key=' + KEY # doesn't seem to respect an order type that we could force by default - my smart idea of using order and publishedBefore doesn't work: https://issuetracker.google.com/issues/128673552
    if pageToken != '':
        url += '&pageToken=' + pageToken
    if publishedBefore != '':
        url += '&publishedBefore=' + publishedBefore
    content = getURL(url)
    callsIndex += 1
    data = json.loads(content)
    if totalResults == 0:
        totalResults = data['pageInfo']['totalResults']

    videosIdsLen = len(videosIds)
    #unqiueVideosIds = set(videosIds)
    #unqiueVideosIdsLen = len(unqiueVideosIds)
    #print(callsIndex, totalResults // maxResults, videosIdsLen, unqiueVideosIdsLen)
    print(callsIndex, totalResults // maxResults, videosIdsLen)
    nextPageToken = pageToken
    if 'nextPageToken' in data:
        nextPageToken = data['nextPageToken']
    items = data['items']
    itemsLen = len(items)
    if itemsLen != maxResults:
        #print(url)
        if callsIndex < totalResults // maxResults:
            publishedBefore = items[-1]['snippet']['publishedAt']
            nextPageToken = ''
    for itemsIndex in range(itemsLen):#maxResults):
        item = items[itemsIndex]
        id = item['id']
        kind = id['kind'].replace('youtube#', '')
        if kind == 'video':
            videoId = id['videoId']
            snippet = item['snippet']
            videoTitle = snippet['title']
            if not videoId in videosIds:
                videosIds += [videoId]
            #if not videoTitle in videosTitles: # otherwise might have videos with same title
                videosTitles += [videoTitle]
                f = open('videos/' + youtuberId + '.txt', 'a', encoding = 'utf-8')
                publishedAt = snippet['publishedAt']
                f.write(videoId + ' ' + publishedAt + ' ' + videoTitle + "\n")
                f.close()
                print(videoId + ' ' + publishedAt + ' ' + videoTitle)
        elif kind == 'playlist':
            playlistId = id['playlistId']
            playlistsIds += [playlistId]
        else:
            print(kind)
    if nextPageToken != pageToken:
        return scrap(nextPageToken, publishedBefore)

scrap()

videosIdsLen = len(videosIds)
#unqiueVideosIds = set(videosIds)
#unqiueVideosIdsLen = len(unqiueVideosIds)
#print(videosIdsLen, unqiueVideosIdsLen)
print(unqiueVideosIds)

playlistsIdsLen = len(playlistsIds)
#uniquePlaylistsIds = set(playlistsIds)
#uniquePlaylistsIdsLen = len(uniquePlaylistsIds)
#print(playlistsIdsLen, uniquePlaylistsIdsLen)
print(playlistsIdsLen)

f = open('videos/' + youtuberId + '.txt', 'w')

f.close()

