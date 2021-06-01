import sys
from typing import Tuple, Dict, Any, List

sys.path.append("../../cmake-build-debug")
sys.path.append("cmake-build-debug")
from bindings import Car


class RoutingRequest:
    @staticmethod
    def coordinates(coords: List[float]):
        if coords:
            return coords[0], coords[1]
        else:
            return None

    def __init__(self, j: Dict[str, Any]):
        self.source_charger_id: int = j['source']['chargerID']
        self.destination_charger_id: int = j['destination']['chargerID']
        self.source_coordinates: Tuple[float, float] = self.coordinates(j['source']['coordinates'])
        self.destination_coordinates: Tuple[float, float] = self.coordinates(j['destination']['coordinates'])
        self.soc_min: float = j['config']['socMin']
        self.soc_max: float = j['config']['socMax']
        self.start_soc: float = j['config']['startSoc']
        self.min_end_soc: float = j['config']['endSoc']
        self.car_id: int = j['car']['carID']
        self.custom_car: Dict[str, Any] = j['car']['custom']
        self.charger_overhead: int = int(j['config']['charger_overhead_in_minutes']) * 60  # convert minutes to seconds

    def car(self):
        if self.car_id == 0:
            return Car(self.start_soc,
                       self.soc_min,
                       self.soc_max,
                       self.min_end_soc,
                       self.custom_car['BatteryCapacity'],
                       self.custom_car['EnergyConsumption'],
                       self.custom_car['CrossSectionalArea'],
                       self.custom_car['RollingResistanceCoeff'],
                       self.custom_car['DragCoeff'],
                       self.custom_car['Mass'],
                       self.custom_car['IdleConsumption'],
                       self.custom_car['DriveTrainEfficiency'],
                       {},
                       self.charger_overhead)
        elif self.car_id == 1:
            return Car.TeslaModel3(self.start_soc, self.soc_min, self.soc_max, self.min_end_soc, self.charger_overhead)
        elif self.car_id == 2:
            return Car.RenaultZoe(self.start_soc, self.soc_min, self.soc_max, self.min_end_soc, self.charger_overhead)
        else:
            return None

    def source_is_charger(self) -> bool:
        return self.source_coordinates is None

    def destination_is_charger(self) -> bool:
        return self.destination_coordinates is None

    def is_between_chargers(self):
        return self.source_coordinates is None and self.destination_coordinates is None
