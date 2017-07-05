#!/usr/bin/env python3

import sys
import socket
import subprocess
import time
import platform
import argparse

def test(testFunction):
    resultTest = testFunction()
    print("\n{0} is {1}passed\n".format(testFunction.__name__,'' if resultTest else 'not '))
    return resultTest

def testPortListening():
    if DEBUG:
        print(sys._getframe().f_code.co_name)
    command = [path, port]
    result = subprocess.Popen(command, stdout=subprocess.PIPE)
    clientSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    time.sleep(0.1)
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
    if DEBUG:
        print(sys._getframe().f_code.co_name)
    command = [path, port]
    result = subprocess.Popen(command, stdout=subprocess.PIPE)
    time.sleep(0.1)
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
    if DEBUG:
        print(sys._getframe().f_code.co_name)
    usedSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    usedSocket.sendto("q".encode(), serverEndPoint)
    usedPort = usedSocket.getsockname()[-1]

    command = [path, str(usedPort)]
    result = subprocess.Popen(command, stdout=subprocess.PIPE)
    clientSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    time.sleep(0.1)
    messages = ['Hello', 'World', 'q']
    if DEBUG:
        for m in messages:
            print('< ', m)
    for m in messages:
        clientSocket.sendto(m.encode(), serverEndPoint)
    return result.returncode != 0

def testPortListeningUnavailablePort():
    if DEBUG:
        print(sys._getframe().f_code.co_name)
    osType = platform.system()
    if DEBUG:
        print("OS is", osType)
    if osType != 'Linux':
        return True
    command = [path, '54']
    result = subprocess.Popen(command, stdout=subprocess.PIPE)
    clientSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    time.sleep(0.1)
    return result.returncode != 0

def testPortListeningSpecificInterface():
    if DEBUG:
        print(sys._getframe().f_code.co_name)
    #TODO
    pass

def testEcho():
    if DEBUG:
        print(sys._getframe().f_code.co_name)
    command = [path, port]
    result = subprocess.Popen(command, stdout=subprocess.PIPE)
    clientSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    time.sleep(0.1)
    messages = ['Hello', 'World', 'q']
    if DEBUG:
        for m in messages:
            print('< ', m)
    for m in messages:
        clientSocket.sendto(m.encode(), serverEndPoint)

    data = [clientSocket.recv(len(m)).decode() for m in messages]
    if DEBUG:
        for d in data:
            print('> ', d)
    return data == messages

def testEchoMultiConnection():
    if DEBUG:
        print(sys._getframe().f_code.co_name)
    command = [path, port]
    result = subprocess.Popen(command, stdout=subprocess.PIPE)
    time.sleep(0.1)
    messages = ['Hello', 'World', 'q']
    amountClients = 3
    sockets = [socket.socket(socket.AF_INET, socket.SOCK_DGRAM) for _ in range(amountClients)]
    for m in messages:
        for sock in sockets:
            if DEBUG:
                print('< ', m)
            sock.sendto(m.encode(), serverEndPoint)

    data = [[sock.recv(len(m)).decode() for m in messages] for sock in sockets]
    if DEBUG:
        print("Data is", data)
    return all(d == messages for d in data)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument('-a','--address', required='True', help='IP address')
    parser.add_argument('-p', '--port', required='True', help='Port of SipServer')
    parser.add_argument('-s', '--sipserver', required='True', help='Path to SipServer')
    parser.add_argument('-i', '--interface', help='Network interface')
    parser.add_argument('-l', '--loglevel', help='Logger level')
    args = parser.parse_args()

    address = args.address
    port = args.port
    path = args.sipserver
    interface = args.interface
    DEBUG = True if args.loglevel else False

    serverEndPoint = (address, int(port))
    print(address, port, path)
    test(testPortListening)
    test(testPortListeningMultiConnection)
    test(testPortListeningUnavailablePort)
    test(testPortListeningUsedPort)
    test(testEcho)
    test(testEchoMultiConnection)
