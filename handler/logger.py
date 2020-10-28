import logging

LOGLEVEL = logging.INFO
FORMAT = "%(asctime)s %(levelname)s:%(name)s %(message)s"
LOGGER_NAME = "EvaCockpit"
IS_INIT = False

def init_logger():
    logger = logging.getLogger(LOGGER_NAME)
    handler = logging.StreamHandler()
    formatter = logging.Formatter(FORMAT)
    handler.setFormatter(formatter)
    handler.setLevel(LOGLEVEL)
    logger.addHandler(handler)
    logger.setLevel(LOGLEVEL)
    logger.info("Logs start")

def get_logger():
    global IS_INIT
    if not IS_INIT:
        init_logger()
        IS_INIT = True
    return logging.getLogger(LOGGER_NAME)
