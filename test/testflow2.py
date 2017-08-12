import socket
import subprocess
import time
import platform
import logging

from config import Config
from utils import timeout, printName, handleLogDir

config = Config()

address = config.address
port = config.port
path = config.path
interface = config.interface
serverEndPoint = config.serverEndPoint

TIMEOUT_LIMIT = 5
TEST_MESSAGES = ['Hello', 'World', 'q']

def process(command, multiConnection=False):
    result = []
    commandResult = subprocess.Popen(command)
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


@handleLogDir
@printName
@timeout(TIMEOUT_LIMIT)
def inMessageInLog():
    result, clientSocket = process([path, '-p', port, '-l', 'INFO'])

    for m in TEST_MESSAGES:
        logging.debug(f'< {m}')
        clientSocket.sendto(m.encode(), serverEndPoint)

    logFile = open('./logs/myeasylog.log')
    print(logFile)

    reason = None
    if logging.getLogger().getEffectiveLevel() == logging.DEBUG:
        for line in logFile:
            logging.debug(f'log: {line[:-1]}')

    logFile = open('./logs/myeasylog.log')
    isAllMessagedLogged = all(any(line.find(x) != -1 for line in logFile) for x in TEST_MESSAGES)
    if not isAllMessagedLogged:
        reason = 'Not all input messaged are logged'
    return isAllMessagedLogged, None


@handleLogDir
@printName
@timeout(TIMEOUT_LIMIT)
def outMessageInLog():
    pass


@handleLogDir
@printName
@timeout(TIMEOUT_LIMIT)
def usedPortInLog():
    pass


@handleLogDir
@printName
@timeout(TIMEOUT_LIMIT)
def unavailablePortInLog():
    pass


@handleLogDir
@printName
@timeout(TIMEOUT_LIMIT)
def unavailableInterfaceLog():
    pass

tests = [inMessageInLog]