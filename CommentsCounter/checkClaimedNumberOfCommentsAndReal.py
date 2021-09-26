import json, requests

videoId = 'XdGE3AW-qdw'

KEY = 'KEY'
REAL_KEY = 'YOUR_API_KEY'

def getURL(url):
    url = url.replace(KEY, REAL_KEY)
    #print(url)
    f = requests.get(url)
    content = f.text
    return content

url = 'https://www.googleapis.com/youtube/v3/videos?part=statistics&id=' + videoId + '&key=' + KEY
content = getURL(url)
data = json.loads(content)
claimedCommentsNumber = int(data['items'][0]['statistics']['commentCount'])
print('claimedCommentsNumber: ' + str(claimedCommentsNumber)) # same on YouTube graphically on a recent video

realCommentsNumber = 0

def countComments(commentId, pageToken = ''):
    global realCommentsNumber
    url = 'https://www.googleapis.com/youtube/v3/comments?part=snippet&parentId=' + commentId + '&maxResults=100&key=' + KEY
    if pageToken != '':
        url += '&pageToken=' + pageToken
    content = getURL(url)
    data = json.loads(content)
    nextPageToken = data['nextPageToken'] if 'nextPageToken' in data else pageToken
    items = data['items']
    itemsLen = len(items)
    realCommentsNumber += itemsLen
    if nextPageToken != pageToken:
        countComments(commentId, nextPageToken)

def scrape(pageToken = ''):
    global realCommentsNumber
    url = 'https://www.googleapis.com/youtube/v3/commentThreads?part=replies,snippet&videoId=' + videoId + '&maxResults=100&key=' + KEY
    if pageToken != '':
        url += '&pageToken=' + pageToken
    content = getURL(url)
    data = json.loads(content)
    nextPageToken = data['nextPageToken'] if 'nextPageToken' in data else pageToken
    items = data['items']
    for item in items:
        commentId = item['id']
        realCommentsNumber += 1
        if 'replies' in item:
            comments = item['replies']['comments']
            if len(comments) == 5:
                countComments(commentId)
    if nextPageToken != pageToken:
        scrape(nextPageToken)

scrape()

print('realCommentsNumber: ' + str(realCommentsNumber))

