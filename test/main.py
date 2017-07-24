#!/usr/bin/env python3

import sys
import socket
import subprocess
import time
import platform
import argparse

LOCALHOST = '127.0.0.1'
testMessages = ['Hello', 'World', 'q']

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
    name = testFunction.__name__
    if DEBUG:
        print(name)

    resultTest = testFunction()
    print("\n{0} is {1}passed".format(name,'' if resultTest else 'NOT '))
    return resultTest

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
    return all(any(r.find(m) != -1 for r in stdoutResult) for m in testMessages)

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
    return all(all(any(r.find(m) != -1 for r in stdoutResult) for m in testMessages) for s in sockets)

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
    return result.returncode != 0

def portListeningUnavailablePort():
    osType = platform.system()
    if DEBUG:
        print("OS is", osType)
    if osType != 'Linux':
        return True

    result, _ = process([path, '-p', '54'])
    return result.returncode != 0

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

    return all(any(r.find(m) != -1 for r in stdoutResult) for m in testMessages)

def portListeningSpecificInterfaceUnavailable():
    result, _ = process([path, '-p', '0', '-n', '1.2.3.4'])
    return result.returncode != 0

def echoServer():
    _, clientSocket = process([path, '-p', port])
    
    if DEBUG:
        for m in testMessages:
            print('< ', m)
    for m in testMessages:
        clientSocket.sendto(m.encode(), serverEndPoint)

    data = [clientSocket.recv(len(m)).decode() for m in testMessages]
    if DEBUG:
        for d in data:
            print('> ', d)
    return data == testMessages

def echoMultiConnection():
    _, sockets = process([path, '-p', port], True)

    for m in testMessages:
        for sock in sockets:
            if DEBUG:
                print('< ', m)
            sock.sendto(m.encode(), serverEndPoint)

    data = [[sock.recv(len(m)).decode() for m in testMessages] for sock in sockets]
    if DEBUG:
        print("Data is", data)
    return all(d == testMessages for d in data)


def main():
    tests = [portListening,
             portListeningMultiConnection,
             portListeningUnavailablePort,
             portListeningUsedPort,
             portListeningSpecificInterface,
             portListeningSpecificInterfaceUnavailable]

    results = [test(name) for name in tests]
    if all(results):
        print('\nAll tests are passed')
    elif any(results):
        print('\nSome tests failed')
    else:
        print("\nAll tests are failed")

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
