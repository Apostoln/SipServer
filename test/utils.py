import functools
import signal
import sys

from config import Config
config = Config()
DEBUG = config.DEBUG

def freePort():
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.bind(('', 0))
        _, port = sock.getsockname()
    return port

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

def printConsoleOut(result):
    if DEBUG:
        stdoutResult = [x.decode() for x in result.stdout]
        stderrResult = [x.decode() for x in result.stderr]
        for r in stdoutResult:
            print('> ', r[:-1])
        for e in stderrResult:
            print(e, file=sys.stderr)
