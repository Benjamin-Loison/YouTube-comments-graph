import time
from selenium import webdriver
from selenium.webdriver.firefox.firefox_binary import FirefoxBinary
from selenium.webdriver.common.keys import Keys

isLinux = True

# zoom 50 % on Linux to have 6 per columns may increase speed ?

# by typing https://www.youtube.com/results?search_query=youtuberName&sp=EgIQAg%253D%253D we get number of videos
FranceInter = 'UCJldRgT_D7Am-ErRHQZ90uw'
France24 = 'UCCCPCZNChQdGa9EkATeye4g'
France24Arabic = 'UCdTyuXgmJkG_O8_75eqej-w'
euronewsInFrench = 'UCW2QcKZiU8aUGg4yxCIditg'

youtuberId = FranceInter
binary = 'C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Python\\Libs\\geckodriver-v0.28.0-win64\\geckodriver.exe'
profile = webdriver.FirefoxProfile()
profile.set_preference('permissions.default.image', 2) # to disable loading images with is quite heavy
if isLinux:
    driver = webdriver.Firefox(firefox_profile=profile)
else:
    driver = webdriver.Firefox(executable_path = binary, firefox_profile=profile)#firefox_binary = binary)
url = 'https://www.youtube.com/channel/' + youtuberId + '/videos'
driver.get(url)
elems = driver.find_elements_by_tag_name('button')
#print(elems)
#elemsLen = len(elems)
elem = elems[-1].click()
elem = driver.find_elements_by_tag_name('html')[0]
#beginTime = time.time()
while True:
    elem.send_keys(Keys.PAGE_DOWN) # taking 15 ms  - 104 ms of ping when using browser network tab
    if isLinux:
        time.sleep(0.005)
    else:
        time.sleep(0.1)

#pageSource = driver.page_source # really useful to make such a heavy copy ?
# can't access to driver.page_source maybe because it's too heavy cf at bottom the error:
pageSourceLen = len(driver.page_source)#pageSource)
print('got pageSourceLen')
#print(pageSourceLen)
# href="/watch?v=s-oGodRtgHA"
pattern = 'href="/watch?v='
pageSourceParts = driver.page_source.split(pattern)#pageSource.split(pattern)
print('got pageSourceParts')
pageSourcePartsLen = len(pageSourceParts)
print('got pageSourcePartsLen')
videosIds = []
for pageSourcePartsIndex in range(pageSourcePartsLen):
    pageSourcePart = pageSourceParts[pageSourcePartsIndex]
    pageSourcePartParts = pageSourcePart.split('"')
    pageSourcePartPartsLen = len(pageSourcePartParts)
    videoId = pageSourcePartParts[0]
    videosIds += [videoId]
    #print(videoId)
print('got videosIds')

videosIdsLen = len(videosIds)
uniqueVideosIds = set(videosIds)
uniqueVideosIdsLen = len(uniqueVideosIds)
print(videosIdsLen, uniqueVideosIdsLen) # I don't know why it's not a number which has 6 as factor because graphically each line has 6 columns and not all videos were loaded but it's maybe better to have almost all videos or a bit more than just a quarter for France 24 arabic for instance
# and first and last ids are correct france inter videos ids
#endTime = time.time()
#print(endTime - beginTime)
driver.close()

"""
>>> len(driver.page_source)
Traceback (most recent call last):
  File "<console>", line 1, in <module>
  File "/usr/local/lib/python3.7/dist-packages/selenium/webdriver/remote/webdriver.py", line 679, in page_source
    return self.execute(Command.GET_PAGE_SOURCE)['value']
  File "/usr/local/lib/python3.7/dist-packages/selenium/webdriver/remote/webdriver.py", line 321, in execute
    self.error_handler.check_response(response)
  File "/usr/local/lib/python3.7/dist-packages/selenium/webdriver/remote/errorhandler.py", line 242, in check_response
    raise exception_class(message, screen, stacktrace)
selenium.common.exceptions.WebDriverException: Message: [Exception... "Failure"  nsresult: "0x80004005 (NS_ERROR_FAILURE)"  location: "JS frame :: chrome://marionette/content/proxy.js :: sendReply_ :: line 287"  data: no]
"""