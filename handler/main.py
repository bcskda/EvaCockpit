import argparse
import protocol
import sys
from config import setup
from logger import init_logger, get_logger
from serial import Serial

logger = get_logger(__name__)

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
    with Serial(args.dev, config.baud_rate, timeout=config.timeout) as dev:
        while True:
            hello_message = protocol.init(dev)
            logger.info("Received hello: %s", hello_message)
            try:
                while True:
                    magic, payload = protocol.readline(dev)
                    if magic == b"M":
                        logger.info("Recv message %s", payload)
                    elif magic == b"C":
                        cmd = int(payload.decode().strip())
                        logger.info("Recv command %s", cmd)
                        config.commands[cmd].run(log=True)
                    else:
                        raise ValueError(f"main: unexpected magic {magic}, mesg {payload}")
            except KeyboardInterrupt:
                break
            except Exception as e:
                logger.warning(f"Unhandled exception: {e}. Restarting.")
                continue

if __name__ == "__main__":
    main(parse_cmdline(sys.argv[1:]))
