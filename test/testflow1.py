import socket
import subprocess
import time
import platform

from config import Config
from utils import timeout, printName

config = Config()

address = config.address
port = config.port
path = config.path
interface = config.interface
DEBUG = config.DEBUG
serverEndPoint = config.serverEndPoint

TIMEOUT_LIMIT = 5
testMessages = ['Hello', 'World', 'q']

def process(command, multiConnection=False):
    result = []
    commandResult = subprocess.Popen(command, stdout=subprocess.PIPE)
    result.append(commandResult)

    if not multiConnection:
        clientSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        result.append(clientSocket)
    else:
        amountClients = 3
        sockets = [socket.socket(socket.AF_INET, socket.SOCK_DGRAM) for _ in range(amountClients)]
        result.append(sockets)

    time.sleep(0.1)
    return tuple(result)


@printName
@timeout(TIMEOUT_LIMIT)
def portListening():
    print(path, port)
    result, clientSocket = process([path, '-p', port])
    if DEBUG:
        for m in testMessages:
            print('< ', m)
    for m in testMessages:
        clientSocket.sendto(m.encode(), serverEndPoint)
    stdoutResult = [x.decode() for x in result.stdout]
    if DEBUG:
        for r in stdoutResult:
            print('> ', r[:-1])

    returncode = result.poll()
    if returncode:
        reason = f'Return code: {returncode}'
        return False, reason
    isAllMessagesDelivered = all(any(r.find(m) != -1 for r in stdoutResult) for m in testMessages)
    reason = 'All messages are not delivered' if isAllMessagesDelivered else None
    return isAllMessagesDelivered, reason


@printName
@timeout(TIMEOUT_LIMIT)
def portListeningMultiConnection():
    result, sockets = process([path, '-p', port], True)
    for m in testMessages:
        for sock in sockets:
            if DEBUG:
                print('< ', m)
            sock.sendto(m.encode(), serverEndPoint)
    stdoutResult = [x.decode() for x in result.stdout]
    if DEBUG:
        for r in stdoutResult:
            print('> ', r[:-1])

    returncode = result.poll()
    if returncode:
        reason = f'Return code: {returncode}'
        return False, reason
    isAllMessagesDelivered = all(all(any(r.find(m) != -1 for r in stdoutResult) for m in testMessages) for s in sockets)
    reason = 'All messages are not delivered' if isAllMessagesDelivered else None
    return isAllMessagesDelivered, reason


@printName
@timeout(TIMEOUT_LIMIT)
def portListeningUsedPort():
    usedSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    usedSocket.sendto("q".encode(), serverEndPoint)
    usedPort = usedSocket.getsockname()[-1]

    result, clientSocket = process([path, '-p', str(usedPort)])
    if DEBUG:
        for m in testMessages:
            print('< ', m)
    for m in testMessages:
        clientSocket.sendto(m.encode(), serverEndPoint)

    returncode = result.poll()
    return returncode != 0, None


@printName
@timeout(TIMEOUT_LIMIT)
def portListeningUnavailablePort():
    osType = platform.system()
    if DEBUG:
        print("OS is", osType)
    if osType != 'Linux':
        return True
    result, _ = process([path, '-p', '54'])
    returncode = result.poll()
    return returncode != 0, None


@printName
@timeout(TIMEOUT_LIMIT)
def portListeningSpecificInterface():
    networkInterface = interface
    endPoint = (networkInterface, serverEndPoint[1])

    result, clientSocket = process([path, '-p', port, '-n', networkInterface])
    if DEBUG:
        for m in testMessages:
            print('< ', m)
    for m in testMessages:
        clientSocket.sendto(m.encode(), endPoint)

    stdoutResult = [x.decode() for x in result.stdout]

    if DEBUG:
        for r in stdoutResult:
            print('> ', r[:-1])

    returncode = result.poll()
    if returncode:
        reason = f
        'Return code: {returncode}'
        return False, reason

    isAllMessagesDelivered = all(any(r.find(m) != -1 for r in stdoutResult) for m in testMessages)
    reason = 'All messages are not delivered' if isAllMessagesDelivered else None
    return isAllMessagesDelivered, reason


@printName
@timeout(TIMEOUT_LIMIT)
def portListeningSpecificInterfaceUnavailable():
    result, _ = process([path, '-p', '0', '-n', '1.2.3.4'])
    returncode = result.poll()
    return returncode != 0, None


@printName
@timeout(TIMEOUT_LIMIT)
def echoServer():
    result, clientSocket = process([path, '-p', port])

    if DEBUG:
        for m in testMessages:
            print('< ', m)
    for m in testMessages:
        clientSocket.sendto(m.encode(), serverEndPoint)

    data = [clientSocket.recv(len(m)).decode() for m in testMessages]
    if DEBUG:
        for d in data:
            print('> ', d)

    returncode = result.poll()
    if returncode:
        reason = f
        'Return code: {returncode}'
        return False, reason

    isAllMessagesDelivered = data == testMessages
    reason = 'All messages are not delivered' if isAllMessagesDelivered else None
    return isAllMessagesDelivered, reason


@printName
@timeout(TIMEOUT_LIMIT)
def echoMultiConnection():
    result, sockets = process([path, '-p', port], True)

    for m in testMessages:
        for sock in sockets:
            if DEBUG:
                print('< ', m)
            sock.sendto(m.encode(), serverEndPoint)

    data = [[sock.recv(len(m)).decode() for m in testMessages] for sock in sockets]
    if DEBUG:
        print("Data is", data)

    returncode = result.poll()
    if returncode:
        reason = f
        'Return code: {returncode}'
        return False, reason

    isAllMessagesDelivered = all(d == testMessages for d in data)
    reason = 'All messages are not delivered' if isAllMessagesDelivered else None
    return isAllMessagesDelivered, reason


tests = [portListening,
         portListeningMultiConnection,
         portListeningUnavailablePort,
         portListeningUsedPort,
         portListeningSpecificInterface,
         portListeningSpecificInterfaceUnavailable,
         echoServer,
         echoMultiConnection]