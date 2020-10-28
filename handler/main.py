import argparse
import sys
import time
from config import setup
from logger import init_logger, get_logger
import serial

logger = get_logger()

def parse_cmdline(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--config", required=True, help="Path to config")
    parser.add_argument("-d", "--dev", required=True, help="Path to serial dev")
    parser.add_argument("--params", metavar="params", nargs="*",
                        help="Extra parameters for commands")
    return parser.parse_args(argv)

def main(args):
    config = setup(args.config, args.params)
    logger.info(config)
    logger.info("Watching serial %s", args.dev)
    with serial.Serial(args.dev, config.baud_rate, timeout=100) as dev:
        # Init
        while True:
            dev.write("R".encode());
            code = dev.read(1)
            if code == b"I":
                logger.info("Received [I]nit request")
                break
            else:
                logger.warning("Expected [I]nit, got %s", code)
                time.sleep(0.217)
        dev.write("I".encode())
        try:
            while True:
                logger.info(dev.readline())
        except KeyboardInterrupt:
            pass

if __name__ == "__main__":
    main(parse_cmdline(sys.argv[1:]))
