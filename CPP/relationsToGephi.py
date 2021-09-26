import os

path = 'C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\CPP\\'

os.chdir(path)

nodes = []
nodesIndexes = {}
relations = []

def addNode(node):
    global nodes
    if not node in nodesIndexes:
        nodesIndexes[node] = len(nodes)
        nodes += [node]

for r, d, files in os.walk('channels'):
    filesLen = len(files)
    for filesIndex in range(filesLen):
        print(str(filesIndex) + ' / ' + str(filesLen))
        file = files[filesIndex]
        #print(file)
        youtuberId = file[:-4]

        f = open('channels\\' + file)#youtuberId + '.txt')
        lines = f.readlines()
        f.close()

        print('channel file loaded')

        linesLen = len(lines)
        slice = linesLen // 100
        for linesIndex in range(linesLen):
            line = lines[linesIndex]
            if line[-1] == "\n":
                line = line[:-1]
            lineParts = line.split()
            src, dest = lineParts[0], lineParts[1]
            addNode(dest)
            addNode(src)
            srcId = nodesIndexes[src]
            destId = nodesIndexes[dest]
            weight = int(lineParts[2])
            relations += [[srcId, destId, weight]]

        print('nodes and relations loaded')

nodesLen = len(nodes)
f = open('all.gexf', 'w')
f.write('<?xml version="1.0" encoding="UTF-8"?>\n<gexf xmlns:viz="http:///www.gexf.net/1.1draft/viz" version="1.1" xmlns="http://www.gexf.net/1.1draft">\n<meta lastmodifieddate="2010-03-03+23:44">\n<creator>Gephi 0.7</creator>\n</meta>\n<graph defaultedgetype="undirected" idtype="string" type="static">\n<nodes count="' + str(nodesLen) + '">\n')

for nodesIndex in range(nodesLen):
    node = nodes[nodesIndex]
    f.write('<node id="' + str(nodesIndex) + '.0" label="' + node + '"/>\n')
f.write('</nodes>\n')

print('nodes written')

relationsLen = len(relations)
f.write('<edges count="' + str(relationsLen) + '">\n')
for relationsIndex in range(relationsLen):
    relation = relations[relationsIndex]
    src, dest, weight = relation

    toWrite = '<edge id="' + str(relationsIndex) + '" source="' + str(src) + '.0" target="' + str(dest) + '.0"'
    if weight > 1:
        toWrite += ' weight="' + str(weight) + '.0"'
    toWrite += '/>\n'
    f.write(toWrite)

print('relations written')
f.write('</edges>\n</graph>\n</gexf>\n')

f.close()

