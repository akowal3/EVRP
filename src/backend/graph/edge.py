import sys

from src.backend.graph.charger import Charger

sys.path.append("../../cmake-build-debug")
sys.path.append("cmake-build-debug")
from bindings import BuildingEdge


class Edge:
    def __init__(self, src: Charger, dst: Charger, distance_m: int, duration_s: int) -> None:
        self.source_charger = src
        self.destination_charger = dst
        self.distance_m = distance_m
        self.duration_s = duration_s

    def speed_ms(self) -> float:
        return self.distance_m / self.duration_s

    def speed_kmh(self) -> float:
        return 3.6 * self.speed_ms()

    def distance_km(self) -> float:
        return self.distance_m / 1000

    def to_cpp(self) -> BuildingEdge:
        return BuildingEdge(
            self.source_charger.internalID,
            self.destination_charger.internalID,
            int(self.distance_km()),
            int(self.speed_kmh())
        )
