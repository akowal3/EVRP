from typing import Dict


class Waypoint:
    def __init__(self) -> None:
        self.latitude: float = 0.0
        self.longitude: float = 0.0
        self.startSoC: float = 0.0
        self.endSoC: float = 0.0
        self.chargerName: str = ''
        self.chargingTime: int = 0
        self.osmChargerID: int = 0

    def toJSON(self) -> Dict:
        return {
            'latitude': self.latitude,
            'longitude': self.longitude,
            'startSoC': self.startSoC,
            'endSoC': self.endSoC,
            'chargerName': self.chargerName,
            'chargingTime': self.chargingTime,
            'chargerID': self.osmChargerID
        }
