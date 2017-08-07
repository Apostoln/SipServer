#!/usr/bin/env python3

import argparse

from utils import freePort
from config import Config

LOCALHOST = '127.0.0.1'

def test(testFunction):
    resultTest, reason = testFunction()
    print("\n{0} is {1}passed".format(testFunction.__name__, '' if resultTest else 'NOT '))
    if not resultTest:
        print('Reason:', reason)
    return resultTest

def main():
    results = [test(name) for name in testflow1.tests]

    if all(results):
        print('\nAll tests are passed')
    elif any(results):
        print('\nSome tests failed')
    else:
        print("\nAll tests are failed")
    print('{0}/{1}'.format(results.count(True), len(testflow1.tests)))

if __name__ == '__main__':
    parser = argparse.ArgumentParser()

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
    config.DEBUG = True if args.loglevel else False
    config.serverEndPoint = (config.address, int(config.port))

    import testflow1

    main()
