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

videosIds, playlistsIds = [], []
callsIndex = 0

def scrap(pageToken = ''):
    global callsIndex, videosIds, playlistsIds # what is the difference between callsIndex and videosIds and youtuberId, the last one is just a string so it's ok ? well in fact maxResults has the same type as callsIndex u_u
    # &type=video
    url = 'https://www.googleapis.com/youtube/v3/search?channelId=' + youtuberId + '&maxResults=' + str(maxResults) + '&type=video&key=' + KEY
    #print(url)
    #exit(42)
    if pageToken != '':
        url += '&pageToken=' + pageToken
    content = getURL(url)
    callsIndex += 1
    data = json.loads(content)
    print(callsIndex, data['pageInfo']['totalResults'] // 50)
    nextPageToken = pageToken
    if 'nextPageToken' in data:
        nextPageToken = data['nextPageToken']
    else:
        print('bruh')
    items = data['items']
    itemsLen = len(items)
    if itemsLen != maxResults:
        print(url)
    for itemsIndex in range(itemsLen):#maxResults):
        item = items[itemsIndex]
        id = item['id']
        kind = id['kind'].replace('youtube#', '')
        if kind == 'video':
            videoId = id['videoId']
            videosIds += [videoId]
        elif kind == 'playlist':
            playlistId = id['playlistId']
            playlistsIds += [playlistId]
        else:
            print(kind)
    if nextPageToken != pageToken:
        return scrap(nextPageToken)

scrap()

# it seems that YouTube claimed videos number between user interface and API differs
# YouTube only giving 12 first pages of results u_u
# maybe linked to https://developers.google.com/youtube/v3/docs/search/list channelId
"""
Note: Search results are constrained to a maximum of 500 videos if your request specifies a value for the channelId parameter and sets the type parameter value to video, but it does not also set one of the forContentOwner, forDeveloper, or forMine filters.
"""
# no because even without type=video we have the same problem u_u
# no order helps
# maybe publishedAfter and publishedBefore parameters can save us
# publishedBefore seems the solution to our problem because with order=date we got latest videos first
# if this approach works it would be interesting to find more channels etc by working on channels etc

videosIdsLen = len(videosIds)
unqiueVideosIds = set(videosIds)
unqiueVideosIdsLen = len(unqiueVideosIds)
print(videosIdsLen, unqiueVideosIdsLen)

playlistsIdsLen = len(playlistsIds)
uniquePlaylistsIds = set(playlistsIds)
uniquePlaylistsIdsLen = len(uniquePlaylistsIds)
print(playlistsIdsLen, uniquePlaylistsIdsLen)

f = open('videos/' + youtuberId + '.txt', 'w')

f.close()

