#!/usr/bin/python3

import os

# copied from gephi script

isWSL = True

def getPath(path):
    if not isWSL:
        return path
    path = path.replace('\\', '/')
    path = path.replace('C:', '/mnt/c')
    return path

path = getPath('C:\\Users\\Benjamin\\Desktop\\BensFolder\\DEV\\Candco\\CPP\\Projects\\YouTubeCommentsGraph\\CPP\\')

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
        file = files[filesIndex]
        #print(str(filesIndex) + ' / ' + str(filesLen) + ' - ' + file)
        youtuberId = file[:-4]

        filePath = getPath('channels\\' + file)
        size = os.path.getsize(filePath)
        #print(size)
        if size == 0 or size >= 10000000:
            continue
        print('really working on: ' + file)
        f = open(filePath)
        lines = f.readlines()
        f.close()

        print('channel file loaded')

        linesLen = len(lines)
        slice = linesLen // 100
        for linesIndex in range(linesLen):
            #if linesIndex == slice:
            #    break
            line = lines[linesIndex]
            if line[-1] == '\n':
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
        #break

nodesLen = len(nodes)
print('nodesLen', nodesLen)
f = open('all.gv.txt', 'w')
f.write('digraph All {\nnode [shape=box];  ')

for nodesIndex in range(nodesLen):
    node = nodes[nodesIndex]
    f.write('u' + str(nodesIndex))#node)
    if nodesIndex < nodesLen - 1:
        f.write('; ')
    else:
        f.write(';\n')

print('nodes written')

relationsLen = len(relations)
print('relationsLen', relationsLen)
for relationsIndex in range(relationsLen):
    relation = relations[relationsIndex]
    src, dest, weight = relation
    #srcName = nodes[src]
    #destName = nodes[dest]
    srcName = 'u' + str(src)
    destName = 'u' + str(dest)
    f.write(srcName + '->' + destName + ';\n')

print('relations written')
f.write('\noverlap=false\nlabel="All"\nfontsize=12\n}"')

f.close()

