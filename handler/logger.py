import logging

LOGLEVEL = logging.INFO
LOGGER_PREFIX = "EvaCockpit"
FORMAT = "%(asctime)s %(levelname)s:{}.%(name)s %(message)s" \
    .format(LOGGER_PREFIX)

g_loggers = dict()

def init_logger(logger: logging.Logger):
    handler = logging.StreamHandler()
    formatter = logging.Formatter(FORMAT)
    handler.setFormatter(formatter)
    handler.setLevel(LOGLEVEL)
    logger.addHandler(handler)
    logger.setLevel(LOGLEVEL)
    logger.info("Logs start")

def get_logger(name: str):
    global g_loggers
    actual_name = "EvaCockpit"
    if not str in g_loggers:
        logger = logging.getLogger(name)
        init_logger(logger)
        g_loggers[name] = logger
    return logging.getLogger(name)
