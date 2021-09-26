#!/usr/bin/python3

import apiclient

KEY = 'YOUR_API_KEY'

youtube = apiclient.discovery.build('youtube', 'v3', developerKey=KEY)

video_id = 'drOy5akB56w'#'oDf6t9Vec-U'

results = youtube.commentThreads().list(
    part="snippet,replies",
    videoId=video_id,
    textFormat="plainText",
    maxResults=100
).execute()

print(results)
