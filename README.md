# YouTube comments graph
[![Travis Build Status](https://travis-ci.com/Benjamin-Loison/YouTube-comments-graph.svg?branch=main)](https://app.travis-ci.com/Benjamin-Loison/YouTube-comments-graph)
[![Mac](https://github.com/Benjamin-Loison/YouTube-comments-graph/actions/workflows/cpp.yml/badge.svg)](https://github.com/Benjamin-Loison/YouTube-comments-graph/actions/workflows/cpp.yml)

This repository focus on retrieving all comments that are on all French YouTube videos channels

The project currently only manage retrieving, in a multithreaded way (a thread per channel), comments from given YouTube videos channels.

If you are interested in a YouTube graph discovery algorithm, then have a look at [my YouTube captions search engine](https://codeberg.org/Benjamin_Loison/YouTube_captions_search_engine).

A aim was to reproduce a Twitch graph which was shown in a Micode video (https://youtu.be/N61_kHXpaFA&t=226s, 2 000 000 nodes) but for YouTube by using public comments data (viewing data aren't publicly available on YouTube, for top 100 (by views) French YouTube channels, nodes: 19 457 798 nodes, 64 487 685 directed edges). I tried to use gephi and graphviz but for both we don't get very interesting results because for the first one it is a Java light software not able to manage easily more than one thousand nodes. For the second, there are different rendering modes but anyway exporting to a PNG isn't very interesting because if we want to export all data we are restricted to PNG limits ~32 768 x 32 768 and if we work to SVG, the result isn't clear to visualize with more strong than Inkscape viewer, I used Firefox and Chrome, but we can't unzoom as we want (even if we could modify the SVG scale etc the graphical view might not be worth enough). Moreover my heaviest tests were performed with only something like French YouTube channels ranked (by views) between 90 and 100 and it was not an easy task with a 32 Go RAM computer with 8 logical threads (and SSD of course))

TODO: discovery mode (make a depth/breadth-first search from a given set of channels like top 100 (by views) French YouTube channels)
This discovery mode isn't planned to be coded because no particular aim it would fulfill.

Furthermore YouTube Data API v3 was a real headache because of many YouTube end bugs.

Here is a few screens of some graphs generated:

![alt text](https://raw.githubusercontent.com/Benjamin-Loison/YouTube-comments-graph/main/CPP/graphviz/low.jpg)
![alt text](https://raw.githubusercontent.com/Benjamin-Loison/YouTube-comments-graph/main/CPP/graphviz/lowSF.jpg)
![alt text](https://raw.githubusercontent.com/Benjamin-Loison/YouTube-comments-graph/main/CPP/graphviz/lowC.jpg)
![alt text](https://raw.githubusercontent.com/Benjamin-Loison/YouTube-comments-graph/main/CPP/graphviz/lowT.jpg)

Note that you can share easily to anyone a reference to a comment on a video by using an URL like https://www.youtube.com/watch?v=dvecqwfU6xw&lc=Ugxw_nsUNUor9AUEBGp4AaABAg.9fDfvkgiqtW9fDkE2r6Blm (this link can also be obtained from the YouTube UI by right-clicking "... ago"). By doing so the comment will be pinned.

<!--Furthermore YouTube Data API v3 was a real headache because of many bugs like:
- using Search: list we can't recover more than ~600 results (channels/videos)
- too many YouTube end bugs that aren't particularly interesting to notice to users

Many features are also not available like:
- listing all -->
