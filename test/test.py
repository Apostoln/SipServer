#!/usr/bin/env python3

import argparse
import logging
import sys

from utils import freePort
from config import Config

LOCALHOST = '127.0.0.1'


def test(testFunction):
    resultTest, reason = testFunction()
    print("{0} is {1}passed".format(testFunction.__name__, '' if resultTest else 'NOT '))
    if not resultTest:
        print('Reason:', reason)
    return resultTest

def main(tests):
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

    parser.add_argument('-t', '--testflow', required='True', help='Number of test')
    parser.add_argument('-s', '--sipserver', required='True', help='Path to SipServer')
    parser.add_argument('-a', '--address', default=LOCALHOST, help='IP address of SipServer')
    parser.add_argument('-p', '--port', help='Port of SipServer')
    parser.add_argument('-i', '--interface', help='Network interface')
    parser.add_argument('-l', '--loglevel', help='Logger level')

    args = parser.parse_args()

    config = Config()
    config.address = args.address
    config.port = str(args.port if args.port else freePort())
    config.path = args.sipserver
    config.interface = args.interface if args.interface else LOCALHOST
    config.serverEndPoint = (config.address, int(config.port))

    debug = True if args.loglevel else False
    logging.basicConfig(stream=sys.stdout,
                        format='%(message)s',
                        level=(logging.DEBUG if debug else logging.WARNING))

    testflow = None
    testflownumber = 0
    try:
        testflownumber = int(args.testflow)
    except ValueError:
        print('--testflow argument must be integer in range [1,2]')
        exit(1)

    if testflownumber == 1:
        import testflow1
        testflow = testflow1.tests
    elif testflownumber == 2:
        import testflow2
        testflow = testflow2.tests
    else:
        print('--testflow argument must be integer in range [1,2]')
        exit(1)

    main(testflow)
