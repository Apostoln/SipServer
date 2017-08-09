import socket
import subprocess
import time
import platform
import logging
import re
import logging
import subprocess
import time

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
ERROR_MSG_PATTERN = re.compile(R"Exit with error code (?P<errorCode>\d+)")
LOGGER_PATH = config.LOGGER_PATH

def checkReturnCode(clientSocket, correctCode):
    for m in TEST_MESSAGES:
        logging.debug(f'< {m}')
        clientSocket.sendto(m.encode(), serverEndPoint)

    logFile = open(LOGGER_PATH)

    reason = None
    if logging.getLogger().getEffectiveLevel() == logging.DEBUG:
        for line in logFile:
            logging.debug(f'log: {line[:-1]}')

    logFile.seek(0)
    fileText = logFile.read()
    matches = re.findall(ERROR_MSG_PATTERN,fileText)
    print(matches)
    if not matches:
        reason = 'There are no messages about error code in log'
        return False, reason
    returnCode = int(matches[0])
    isReturnCodeCorrect = returnCode == correctCode
    if not isReturnCodeCorrect:
        reason = f'Error code is incorrect. Must be {correctCode}, now {returnCode}'
    return isReturnCodeCorrect, reason

def checkMessageInLog(isInput):
    sign = '>' if input else '<'
    result, clientSocket = process([path, '-p', port, '-l', 'INFO'])

    for m in TEST_MESSAGES:
        logging.debug(f'< {m}')
        clientSocket.sendto(m.encode(), serverEndPoint)

    logFile = open(LOGGER_PATH)

    reason = None
    if logging.getLogger().getEffectiveLevel() == logging.DEBUG:
        for line in logFile:
            logging.debug(f'log: {line[:-1]}')

    logFile.seek(0)
    logFileString = logFile.read()
    isAllMessagedLogged = all(re.findall(f'.+ {sign} {msg}', logFileString) for msg in TEST_MESSAGES)

    if not isAllMessagedLogged:
        reason = 'Not all input messaged are logged'
    return isAllMessagedLogged, reason



def process(command, multiConnection=False):
    result = []
    command.append('-f')
    command.append(LOGGER_PATH)
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
    return checkMessageInLog(isInput=True)


@handleLogDir
@printName
@timeout(TIMEOUT_LIMIT)
def outMessageInLog():
    return checkMessageInLog(isInput=False)


@handleLogDir
@printName
@timeout(TIMEOUT_LIMIT)
def usedPortInLog():
    usedSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    usedSocket.sendto("q".encode(), serverEndPoint)
    usedPort = usedSocket.getsockname()[-1]

    result, clientSocket = process([path, '-p', str(usedPort), '-l', 'DEBUG'])
    return checkReturnCode(clientSocket,correctCode=1)

@handleLogDir
@printName
@timeout(TIMEOUT_LIMIT)
def unavailablePortInLog():
    osType = platform.system()
    logging.debug(f'OS is  {osType}')
    if osType != 'Linux':
        return True
    result, clientSocket = process([path, '-p', '54', '-l', 'DEBUG'])
    return checkReturnCode(clientSocket,correctCode=2)


@handleLogDir
@printName
@timeout(TIMEOUT_LIMIT)
def unavailableInterfaceLog():
    usedSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    usedSocket.sendto("q".encode(), serverEndPoint)
    usedPort = usedSocket.getsockname()[-1]

    result, clientSocket = process([path, '-n', '1.2.3.4', '-l', 'DEBUG', '-c', '1'])
    return checkReturnCode(clientSocket, correctCode=3)

tests = [inMessageInLog, outMessageInLog, usedPortInLog, unavailablePortInLog, unavailableInterfaceLog]




