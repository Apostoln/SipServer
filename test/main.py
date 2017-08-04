#!/usr/bin/env python3

import socket
import subprocess
import time
import platform
import argparse
import functools
import signal

TIMEOUT_LIMIT = 5
LOCALHOST = '127.0.0.1'
testMessages = ['Hello', 'World', 'q']

class TimeoutError(Exception):
    def __init__(self, value='Time out'):
        self.value = value
    def __str__(self):
        return self.value

def timeout(seconds):
    def decorate(func):
        def handler(signum, frame):
            raise TimeoutError('Timeout limit in {0} seconds is exceeded'.format(seconds))

        @functools.wraps(func)
        def wrapped(*args, **kwargs):
            old = signal.signal(signal.SIGALRM, handler)
            signal.alarm(seconds)
            try:
                result = func(*args, **kwargs)
            except TimeoutError as e:
                result = False
                reason = str(e)
                return result, reason
            finally:
                signal.signal(signal.SIGALRM, old)
            return result
        return wrapped
    return decorate

def printName(fn):
    @functools.wraps(fn)
    def wrapped():
        if DEBUG:
            print(fn.__name__)
        return fn()
    return wrapped

def freePort():
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.bind(('', 0))
        _, port = sock.getsockname()
    return port

def process(command, multiConnection = False):
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

def test(testFunction):
    resultTest, reason = testFunction()
    print("\n{0} is {1}passed".format(testFunction.__name__,'' if resultTest else 'NOT '))
    if not resultTest:
        print('Reason:', reason);
    return resultTest

@printName
@timeout(TIMEOUT_LIMIT)
def portListening():
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
        reason = f'Return code: {returncode}'
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
        reason = f'Return code: {returncode}'
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
        reason = f'Return code: {returncode}'
        return False, reason

    isAllMessagesDelivered = all(d == testMessages for d in data)
    reason = 'All messages are not delivered' if isAllMessagesDelivered else None
    return isAllMessagesDelivered, reason

def main():
    tests = [portListening,
             portListeningMultiConnection,
             portListeningUnavailablePort,
             portListeningUsedPort,
             portListeningSpecificInterface,
             portListeningSpecificInterfaceUnavailable,
             echoServer,
             echoMultiConnection]

    results = [test(name) for name in tests]

    if all(results):
        print('\nAll tests are passed')
    elif any(results):
        print('\nSome tests failed')
    else:
        print("\nAll tests are failed")
    print('{0}/{1}'.format(results.count(True), len(tests)))

if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument('-s', '--sipserver', required='True', help='Path to SipServer')
    parser.add_argument('-a', '--address', default=LOCALHOST, help='IP address of SipServer')
    parser.add_argument('-p', '--port', help='Port of SipServer')
    parser.add_argument('-i', '--interface', help='Network interface')
    parser.add_argument('-l', '--loglevel', help='Logger level')
    args = parser.parse_args()

    address = args.address
    port = str(args.port if args.port else freePort())
    path = args.sipserver
    interface = args.interface if args.interface else LOCALHOST
    DEBUG = True if args.loglevel else False
    serverEndPoint = (address, int(port))

    main()
