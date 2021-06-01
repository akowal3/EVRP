from typing import Dict, Tuple


class ResponseWaypoint:
    def __init__(self, location: Tuple[float, float], socIn: float, socOut: float, charging_time: int, osmID: int) -> None:
        self.latitude: float = location[0]
        self.longitude: float = location[1]
        self.socIn: float = socIn
        self.socOut: float = socOut
        self.chargerName: str = ''
        self.chargingTime: int = charging_time
        self.osmChargerID: int = osmID

    def toJSON(self) -> Dict:
        return {
            'latitude': self.latitude,
            'longitude': self.longitude,
            'socIn': self.socIn,
            'socOut': self.socOut,
            'chargerName': self.chargerName,
            'chargingTime': self.chargingTime,
            'chargerID': self.osmChargerID
        }


class ResponseEdge:
    def __init__(self, speed: float, distance: float, energy: float, soc: float) -> None:
        self.speed_kmh: float = speed
        self.distance_km: float = distance
        self.consumed_energy: float = energy
        self.consumed_soc: float = soc

    def toJSON(self) -> Dict:
        return {
            'speed_kmh': self.speed_kmh,
            'distance_km': self.distance_km,
            'consumed_energy': self.consumed_energy,
            'consumed_soc': self.consumed_soc,
        }
