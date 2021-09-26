import urllib.request, json
from urllib.error import HTTPError

KEY = 'KEY'
REAL_KEY = 'YOUR_API_KEY'
playlistId = 'UUJldRgT_D7Am-ErRHQZ90uw'

def getURL(url):
    url = url.replace(KEY, REAL_KEY)
    res = ""
    try:
        res = urllib.request.urlopen(url).read()
    except HTTPError as e:
        res = e.read()
    return res

# let say that the totalResults (19 999) is false and let's compute it "by hand"
slices = 0
sliceSize = 50

def scrap(pageToken = ''):
    global slices
    print(slices, pageToken)
    url = 'https://www.googleapis.com/youtube/v3/playlistItems?part=contentDetails&playlistId=' + playlistId + '&maxResults=' + str(sliceSize) + '&key=' + KEY
    if pageToken != '':
        url += '&pageToken=' + pageToken # hope no 'KEY' in any of about ~400 pageTokens
    content = getURL(url)
    slices += 1
    data = json.loads(content)
    nextPageToken = pageToken
    if 'nextPageToken' in data:
        nextPageToken = data['nextPageToken']
    else:
        print('no next page token')
    if nextPageToken != pageToken:
        scrap(nextPageToken)

scrap()

print(slices * sliceSize)

