import sys
from enum import Enum
from typing import Dict, Any, Tuple

sys.path.append("../../cmake-build-debug")
sys.path.append("cmake-build-debug")
from bindings import Node, charger_type


class ChargerType(Enum):
    SLOW_50KW = 1
    FAST_175KW = 2
    NO_CHARGER = 3


class Charger:

    def __init__(self):
        self.osmID: int = 0
        self.internalID: int = 0
        self.lat: float = 0.0
        self.long: float = 0.0
        self.charger_type: ChargerType = ChargerType.NO_CHARGER

    def __repr__(self) -> str:
        return f"Charger<ID: {self.internalID}/{self.osmID}, LOCATION: ({self.lat}, {self.long}), TYPE: {self.charger_type}>"

    def location(self):
        return self.lat, self.long

    def to_cpp(self) -> Node:
        cpp_charger_type = None
        if self.charger_type is ChargerType.FAST_175KW:
            cpp_charger_type = charger_type.FAST_175KW
        elif self.charger_type is ChargerType.SLOW_50KW:
            cpp_charger_type = charger_type.SLOW_50KW
        elif self.charger_type is ChargerType.NO_CHARGER:
            cpp_charger_type = charger_type.NO_CHARGER

        return Node(self.internalID, 1.0, cpp_charger_type)

    @classmethod
    def from_ocm(cls, res: Dict[str, Any], internalID: int):
        this = cls()

        this.osmID = res['ID']
        this.internalID = internalID
        this.lat = res['AddressInfo']['Latitude']
        this.long = res['AddressInfo']['Longitude']
        this.charger_type = ChargerType.SLOW_50KW

        for c in res['Connections']:
            if c['ConnectionTypeID'] == 27:  # tesla supercharger
                this.charger_type = ChargerType.FAST_175KW
                break

        return this

    @classmethod
    def temp_node(cls, internalID: int, coordinates: Tuple[float, float]):
        this = cls()
        this.internalID = internalID
        this.lat, this.long = coordinates
        this.charger_type = ChargerType.NO_CHARGER
        return this
