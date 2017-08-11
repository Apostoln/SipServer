from config import Config
from utils import timeout, printName, printConsoleOut

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
    commandResult = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
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

def inMessageInLog():
    pass

def outMessageInLog():
    pass

def usedPortInLog():
    pass

def unavailablePortInLog():
    pass

def unavailableInterfaceLog():
    pass

