import os

path = "F:\\YouTube\\CommentsGraph\\details\\"

os.chdir(path)

l = []

print("reading")

f = open("UCWeg2Pkate69NFdBeuRFTAw-names.txt", encoding="utf-8")
lines = f.readlines()
f.close()

print("read", linesLen)

linesLen = len(lines)
for linesIndex in range(linesLen):
    if linesIndex % 10000 == 0:
        print(linesIndex)
    line = lines[linesIndex]
    if line[-1] == "\n":
        line = line[:-1]
    if not line in l:
        l += [line]

print("treated", lLen)

f = open("res.txt", 'w')
lLen = len(l)
for lIndex in range(lLen):
    lEl = l[lIndex]
    f.write(lEl)
    if lIndex < lLen - 1:
        f.write("\n")
f.close()

print("written")

