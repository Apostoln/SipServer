import socket
import subprocess
import time
import platform
import logging
import re

from utils import printName, timeout, handleLogDir
from config import Config
config = Config()
address = config.address
port = config.port
path = config.path
interface = config.interface
serverEndPoint = config.serverEndPoint

TIMEOUT_LIMIT = 10
TEST_SIP_REQUEST = R"""INVITE sip:nikolia@example.com SIP/2.0
Record-Route: <sip:nikolia@10.0.0.10;lr>
Via: SIP/2.0/UDP 10.0.0.10;branch=z9hG4bK3af7.0a6e92f4.0
Via: SIP/2.0/UDP 192.168.0.2:5060;branch=z9hG4bK12ee92cb;rport=5060
From: "78128210000" <sip:78128210000@neutral.com>;tag=as149b2d97
To: <sip:nikolia@example.com>
Contact: <sip:78128210000@neutral.com>
Call-ID: 3cbf958e6f43d91905c3fa964a373dcb@example.com
CSeq: 103 INVITE
Max-Forwards: 16
Date: Wed, 10 Jan 2001 13:16:23 GMT
Allow: INVITE, ACK, CANCEL, OPTIONS, BYE, REFER, SUBSCRIBE, NOTIFY
Supported: replaces
Content-Type: application/sdp
Content-Length: 394

v=0
o=root 3303 3304 IN IP4 10.0.0.10
s=session
c=IN IP4 10.0.0.10
t=0 0
m=audio 40358 RTP/AVP 0 8 101
a=rtpmap:0 PCMU/8000
a=rtpmap:8 PCMA/8000
a=rtpmap:101 telephone-event/8000
a=fmtp:101 0-16
a=silenceSupp:off - - - -
a=sendrecv"""

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
def requestParsing():
    headers = []
    values = []
    for line in TEST_SIP_REQUEST.split('\n')[1:]:
        if not line:
            break
        splited = line.split(':',1)
        headers.append(splited[0])
        values.append(''.join(splited[1:]).lstrip())

    result, clientSocket = process([path, '-p', port])
    clientSocket.sendto(TEST_SIP_REQUEST.encode(), serverEndPoint)

    logging.debug('Send:')
    for line in TEST_SIP_REQUEST.split('\n'):
        logging.debug(f'<{line}')

    data = clientSocket.recv(4096).decode()
    if logging.getLogger().getEffectiveLevel() == logging.DEBUG:
        for d in data.split('\n'):
            logging.debug(f'> {d}')

    reason = ''
    headersLine = None
    valuesLine = None
    try:
        try:
            headersLine = [d for d in data.split('\n') if d.find('Headers') != -1][0]
        except Exception as e:
            reason = 'There are no "Headers" header'
            raise e

        try:
            valuesLine = [d for d in data.split('\n') if d.find('Values') != -1][0]
        except Exception as e:
            reason = 'There are no "Values" header'
            raise e
    except Exception as e:
        return False, reason

    headersFlag = all(headersLine.find(h) != -1 for h in headers)
    valuesFlag = all(valuesLine.find(v) != -1 for v in values)
    reason = ''

    if not headersFlag:
        reason += 'Not all headers are present in Headers header\n'
    if not valuesFlag:
        reason += "Not all values are present in Values header"


    returncode = result.poll()
    isFinishSuccessfully = not bool(returncode)
    if not isFinishSuccessfully:
        reason = f"Return code is {returncode}"
    return isFinishSuccessfully and headersFlag and valuesFlag , reason

tests = [requestParsing]