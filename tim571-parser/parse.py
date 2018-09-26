fileName = 'data.txt'

class TimData:
    def __init__(self):
        self.distance = []
        self.rssi = []

def readData(data):
    file = open(fileName, 'r');
    for line in iter(file.readline, b''):
        isDistance = True
        for word in line.split()[1:]:
            number = word.replace('(', '').replace(')', '')
            try:
                number = int(number)
                data.distance.append(number) if isDistance else data.rssi.append(number)
                isDistance = not isDistance
            except ValueError:
                pass
    file.close()

def createCArray(list):
    result = ''
    for index, item in enumerate(list):
        result += str(item) + ', ' + ('\n' if index % 10 == 9 else '')
    return '{' + result[:-2] + '}'

def main():
    data = TimData()
    readData(data)
    distance = createCArray(data.distance)
    rssi = createCArray(data.rssi)
    print('Distance data:')
    print(distance)
    print('RSSI data:')
    print(rssi)

if __name__ == "__main__":
    main()
