import functools
import signal
import sys
import logging
import socket
import os
import shutil

from config import Config
config = Config()
LOGGER_PATH = config.LOGGER_PATH
LOG_DIR = LOGGER_PATH.split('/')[0]
LOG_DIR = LOG_DIR if LOG_DIR else '.'


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
        if logging.getLogger().getEffectiveLevel() == logging.DEBUG:
            print('\n'+fn.__name__)
        return fn()
    return wrapped

def handleLogDir(fn):
    @functools.wraps(fn)
    def wrapped():
        res = fn()
        shutil.rmtree(LOG_DIR, ignore_errors=True)
        return res
    return wrapped



def printConsoleOut(result):
    if logging.getLogger().getEffectiveLevel() == logging.DEBUG:
        stdoutResult = [x.decode() for x in result.stdout]
        stderrResult = [x.decode() for x in result.stderr]
        for r in stdoutResult:
            logging.debug(f'> {r[:-1]}')
        for e in stderrResult:
            logging.debug(f'stderr: {e}')

