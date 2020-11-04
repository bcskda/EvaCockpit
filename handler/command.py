from dataclasses import dataclass
from subprocess import check_call
from typing import List
from logger import get_logger

logger = get_logger(__name__)

@dataclass
class Command:
    name: str
    cmdline: List[str]
    
    def run(self, /, log=False):
        if log:
            logger.info("Running command %s", self.name)
        cc = check_call(self.cmdline)
        if log:
            logger.info("Exited command %s", self.name)
        return cc

@dataclass
class SystemdService:
    name: str
    
    def start(self, /, log=False):
        if log:
            logger.info("SystemdService starting %s", self.name)
        check_call(["systemctl", "start", name])
        if log:
            logger.info("SystemdService started %s", self.name)
    
    def stop(self, /, log=False):
        if log:
            logger.info("SystemdService stopping %s", self.name)
        check_call(["systemctl", "stop", name])
        if log:
            logger.info("SystemdService stopped %s", self.name)
