import time
from logger import get_logger
from serial import Serial
from typing import Tuple

logger = get_logger(__name__)

SKIP_DELAY_SEC = 0.179
SKIP_TIMEOUT_SEC = 2

INIT_DELAY_SEC = 0.137

PROTOCOL_VERSION = "1"

MAGIC_CMD  =  b"C"
MAGIC_INIT =  b"I"
MAGIC_MESG =  b"M"
MAGIC_PING =  b"P"
MAGIC_RESET = b"R"

def skip_unreceived(dev: Serial, timeout_sec: float):
    """Skip all not-yet-received data"""
    timeout = dev.timeout
    dev.timeout = timeout_sec
    skipped_all = False
    while not skipped_all:
        skip = dev.readline()
        skipped_all = not skip
        if skip:
            logger.info("Skipping unreceived %s", skip)
            time.sleep(SKIP_DELAY_SEC)
    dev.timeout = timeout

def check_version(dev: Serial, expected: str):
    """
    Read version string and check it is as expected
    Throws ValueError if doesn't match
    """
    version = dev.readline().decode().strip()
    if version != expected:
        msg = f"Unsupported version {version}"
        logger.error(msg)
        raise ValueError(msg)
    else:
        logger.info("Protocol version %s", version)

def init(dev: Serial):
    """Init protocol. Returns peer's hello string."""
    logger.info("Starting")
    while True:
        # Skip all unreceived data and send [R]eset
        skip_unreceived(dev, SKIP_TIMEOUT_SEC)
        dev.write(MAGIC_RESET)
        # Try receive [I]nit
        magic = dev.read(1)
        if magic == MAGIC_INIT:
            # Received [I]nit magic.
            # Answer with [I]nit, receive version string.
            logger.info("Received [I]nit magic")
            check_version(dev, PROTOCOL_VERSION)
            dev.write(MAGIC_INIT)
            break
        else:
            # Unexpected magic.
            # [R]eset and retry.
            logger.warning("Expected [I]nit, got %s", magic)
            dev.write(MAGIC_RESET)
            time.sleep(INIT_DELAY_SEC)
    return dev.readline()

def readline(dev) -> Tuple[bytes, str]:
    """
    Read one line, reply with [P]ing
    Returns (magic, payload)
    Throws ValueError if unexpected magic
    """
    magic = dev.read(1)
    if magic in {MAGIC_MESG, MAGIC_CMD}:
        m = dev.readline()
    else:
        warn = f"Unexpected magic {magic}"
        logger.warning(warn)
        raise ValueError(warn)
    dev.write(MAGIC_PING)
    return magic, m
