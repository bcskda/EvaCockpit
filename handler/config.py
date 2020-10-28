import json
from dataclasses import dataclass
from typing import Dict
from command import Command, SystemdService

@dataclass
class Config:
    commands: Dict[int, Command]
    services: Dict[int, SystemdService]
    baud_rate: int = 9600
    timeout: float = None # Wait forever
    
    @classmethod
    def from_conf_object(cls, conf_dict, params):
        commands = dict()
        for idx, cmd in conf_dict["commands"].items():
            cmdline = map(lambda x: x.format(params), cmd["cmdline"])
            commands[int(idx)] = Command(cmd["name"], list(cmdline))
        services = dict()
        for idx, svc in conf_dict["services"].items():
            services[int(idx)] = SystemdService(svc["name"])
        return cls(commands, services)

def setup(config_path, params) -> Config:
    with open(config_path) as conf_file:
        conf_dict = json.loads(conf_file.read())
        return Config.from_conf_object(conf_dict, params)
