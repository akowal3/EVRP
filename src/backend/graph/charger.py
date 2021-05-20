import sys
from enum import Enum
from typing import Dict, Any

sys.path.append("../../cmake-build-debug")
sys.path.append("cmake-build-debug")
from bindings import Node, charger_type


class ChargerType(Enum):
    SLOW_50KW = 1
    FAST_175KW = 2


class Charger:

    def __init__(self, res: Dict[str, Any], internalID: int) -> None:
        self.osmID = res['ID']
        self.internalID = internalID
        self.lat = res['AddressInfo']['Latitude']
        self.long = res['AddressInfo']['Longitude']
        self.charger_type = ChargerType.SLOW_50KW

        for c in res['Connections']:
            if c['ConnectionTypeID'] == 27:  # tesla supercharger
                self.charger_type = ChargerType.FAST_175KW
                break

    def __repr__(self) -> str:
        return f"Charger<ID: {self.internalID}/{self.osmID}, LOCATION: ({self.lat}, {self.long}), TYPE: {self.charger_type}>"

    def location(self):
        return self.lat, self.long

    def to_cpp(self) -> Node:
        return Node(self.internalID, 1.0, charger_type.FAST_175KW if self.charger_type == ChargerType.FAST_175KW else charger_type.SLOW_50KW)
