import sys
fileName = 'data.txt'

prefix = 'lines->'

class Line2d:
    def __init__(self, distance, angle):
        self.distance = distance
        self.angle = angle

class Line:
    def __init__(self, line2d, votes):
        self.line = line2d
        self.votes = votes

    def set_index(self, index):
        self.index = index

    def __str__(self):
        return '{}lines[{}].line.distance = {};\n{}lines[{}].line.angle = {};\n{}lines[{}].votes = {};\n'.format(prefix, self.index, self.line.distance, prefix, self.index, self.line.angle, prefix, self.index, self.votes)

class LinesData:
    def __init__(self):
        self.lines = []

    def append(self, line):
        line.set_index(len(self.lines))
        self.lines.append(line)

    def __str__(self):
        return '{}count = {};\n\n'.format(prefix, len(self.lines)) + '\n'.join(map(str, self.lines))

def readData(data):
    file = open(fileName, 'r')
    for line in iter(file.readline, b''):
        line = line.split()
        if len(line) > 0:
            data.append(Line(Line2d(line[2][:-1], line[4][:-1]), line[6]))
    file.close()

def main():
    data = LinesData()
    readData(data)
    print(str(data))

if __name__ == "__main__":
    main()
