#!/usr/bin/python3

import apiclient

#KEY = 'YOUR_API_KEY'
KEY = "YOUR_API_KEY"

def build_comment(sn):
    print(sn)
    return {'author': sn['authorDisplayName'], 'text': sn['textDisplay']}

counter = 0

def scrape(comments, video_id, f, token=None):
    global counter

    youtube = apiclient.discovery.build('youtube', 'v3', developerKey=KEY)

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

    #print("!")
    #print(results)
    #print("!")
    """for item in results['items']:
        comment = build_comment(item['snippet']['topLevelComment']['snippet'])
        comments.append(comment)
        if 'replies' in item.keys():
            for reply in item['replies']['comments']:
                comment = build_comment(reply['snippet'])
                comments.append(comment)"""

    #comments += [results]
    f.write(str(results) + "\n")

    if 'nextPageToken' in results:
        scrape(comments, video_id, f, results['nextPageToken'])
        #comments.extend(scrape(comments, video_id, f, results['nextPageToken']))

    #return comments

def workVideo(videoId):
    f = open('videos/' + videoId + '.txt', 'w')
    #comments_list = scrape([], videoId, f)
    scrape([], videoId, f)
    #for x in comments_list:
    #    print(x)
    #comments_list_str = str(comments_list)
    #f.write(comments_list_str)
    f.close()

workVideo('_ZPpU7774DQ') # drOy5akB56w oDf6t9Vec-U
