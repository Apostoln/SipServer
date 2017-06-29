import sys
import socket
import subprocess
import time

DEBUG = False

def test(testFunction):
    resultTest = testFunction()
    print("\n{0} is {1}passed\n".format(testFunction.__name__,'' if resultTest else 'not '))
    return resultTest

def testPortListening():
    command = [path, port]
    result = subprocess.Popen(command, stdout=subprocess.PIPE)
    clientSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    time.sleep(0.5)
    messages = ['Hello', 'World', 'q']
    if DEBUG:
        for m in messages:
            print('< ', m)
    for m in messages:
        clientSocket.sendto(m.encode(), serverEndPoint)
    stdoutResult = [x.decode() for x in result.stdout]
    if DEBUG:
        for r in stdoutResult:
            print('> ', r[:-1])
    if all(any(r.find(m) != -1 for r in stdoutResult) for m in messages):
        return True
    return False

def testPortListeningMultiConnection():
    command = [path, port]
    result = subprocess.Popen(command, stdout=subprocess.PIPE)
    time.sleep(0.5)
    messages = ['Hello', 'World', 'q']
    amountClients = 3
    sockets = [socket.socket(socket.AF_INET, socket.SOCK_DGRAM) for _ in range(amountClients)]
    for m in messages:
        for sock in sockets:
            if DEBUG:
                print('< ', m)
            sock.sendto(m.encode(), serverEndPoint)
    stdoutResult = [x.decode() for x in result.stdout]
    if DEBUG:
        for r in stdoutResult:
            print('> ', r[:-1])
    if all(all(any(r.find(m) != -1 for r in stdoutResult) for m in messages) for s in sockets):
        return True
    return False

def testPortListeningUsedPort():
    pass

def testPortListeningSpecificInterface():
    pass

if __name__ == '__main__':
    address = sys.argv[1]
    port = sys.argv[2]
    path = sys.argv[3]
    try:
        DEBUG = sys.argv[4]
    except IndexError:
        DEBUG = False

    serverEndPoint = (address, int(port))
    print(address, port, path)
    test(testPortListening)
    test(testPortListeningMultiConnection)
