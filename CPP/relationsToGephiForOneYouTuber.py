import os

path = 'C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\CPP\\'

os.chdir(path)

youtuberId = 'UCa_m88y-YP42wOdoYaIk6og'#'UCjd32KVfRCli1d9iqo4YZ5A'#'UCGT27y25O1pIVRmdhxliSNQ'#'UC_yP2DpIgs5Y1uWC0T03Chw'#'UCWeg2Pkate69NFdBeuRFTAw'

f = open('channels\\' + youtuberId + '.txt')
lines = f.readlines()
f.close()

print('channel file loaded')

nodes = []
nodesIndexes = {}

def addNode(node):
    global nodes
    #if not node in nodes:
    if not node in nodesIndexes:
        nodesIndexes[node] = len(nodes)
        nodes += [node]

relations = []
linesLen = len(lines)
slice = linesLen // 100
for linesIndex in range(linesLen):
    #if linesIndex % slice == 0:
    #    print(linesIndex // slice)
    line = lines[linesIndex]
    if line[-1] == '\n':
        line = line[:-1]
    lineParts = line.split()
    src, dest = lineParts[0], lineParts[1]
    addNode(dest) # to make youtuber id 0
    addNode(src)
    relations += [[src, dest, lineParts[2]]]

print('nodes and relations loaded')

nodesLen = len(nodes)
f = open('gephi\\' + youtuberId + '.gexf', 'w')
f.write('<?xml version="1.0" encoding="UTF-8"?>\n<gexf xmlns:viz="http:///www.gexf.net/1.1draft/viz" version="1.1" xmlns="http://www.gexf.net/1.1draft">\n<meta lastmodifieddate="2010-03-03+23:44">\n<creator>Gephi 0.7</creator>\n</meta>\n<graph defaultedgetype="undirected" idtype="string" type="static">\n<nodes count="' + str(nodesLen) + '">\n')
for nodesIndex in range(nodesLen):
    node = nodes[nodesIndex]
    f.write('<node id="' + str(nodesIndex) + '.0" label="' + node + '"/>\n')
f.write('</nodes>')

print('nodes written')

relationsLen = len(relations)
f.write('<edges count="' + str(relationsLen ) + '">')
for relationsIndex in range(relationsLen):
    relation = relations[relationsIndex]
    src, dest, weight = relation

    toWrite = '<edge id="' + str(relationsIndex) + '" source="' + str(nodesIndexes[src]) + '.0" target="' + str(nodesIndexes[dest]) + '.0"'
    if int(weight) > 1:
        toWrite += ' weight="' + weight + '.0"' # str()
    toWrite += '/>\n'
    f.write(toWrite)

print('relations written')
f.write('</edges>\n</graph>\n</gexf>\n')

f.close()

# could merge if A answer to B and B answer to A
# or make the graph directed

