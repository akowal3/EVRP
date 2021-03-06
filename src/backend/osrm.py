from time import sleep
from typing import Dict, Any, List, Tuple

import requests

from charger import Charger
from edge import Edge


class OSRM:
    SEC_PER_HOUR = 3600

    def __init__(self, host: str, port: int) -> None:
        self.host = host
        self.port = port
        self.profile = 'car'

    @staticmethod
    def format_coords(coords: List[Tuple]) -> str:
        """
        Formats NumPy array of (lat, long) coordinates into a concatenated string formatted
        for the OSRM server.
        """
        return ";".join([f"{lon:f},{lat:f}" for lat, lon in coords])

    @staticmethod
    def format_options(options: Dict[str, str]) -> str:
        """
        Formats dictionary of additional options to your OSRM request into a
        concatenated string format.
        """
        return "&".join([f"{k}={v}" for k, v in options.items()])

    def make_request(
            self,
            service: str,
            coords: List[Tuple[float, float]],
            options: dict = None
    ) -> Dict[str, Any]:
        """
        Forwards your request to the OSRM server and returns a dictionary of the JSON
        response.
        """
        coords = self.format_coords(coords)
        options = self.format_options(options) if options else ""
        url = f"http://{self.host}:{self.port}/{service}/v1/{self.profile}/{coords}?{options}"
        response = requests.get(url)
        return response.json()

    def route_dt(self, coords: List[Tuple[float, float]]):
        """Returns the distance/time to travel a given route."""
        x = self.make_request(
            service='route',
            coords=coords,
            options={'steps': 'false', 'overview': 'false'}
        )
        x = x['routes'][0]
        return x['distance'], x['duration']

    def route_result(self, coords: List[Tuple[float, float]]):
        """Returns the full json to travel a given route between chargers."""
        return self.make_request(
            service='route',
            coords=coords,
            options={'steps': 'false', 'overview': 'false'}
        )

    def charger_table(self, chargers: List[Charger]):
        res = pd.DataFrame(columns=range(len(chargers)), index=range(len(chargers)))
        for i, src in enumerate(chargers):
            for j, dst in enumerate(chargers):
                res[i][j] = self.route_dt([src.location(), dst.location()])
                if j % 40 == 0:
                    sleep(0.1)

        return res

    def edge_list(self, chargers_from: List[Charger], chargers_to: List[Charger]):
        res = []
        distance_lower_bound = 10 * 1000
        distance_upper_bound = 600 * 1000
        for src in chargers_from:
            for dst in chargers_to:
                if src.internalID != dst.internalID:
                    distance_m, duration_s = self.route_dt([src.location(), dst.location()])
                    if distance_lower_bound <= distance_m <= distance_upper_bound and duration_s >= 10:
                        res.append(Edge(src, dst, distance_m, duration_s))
                sleep(0.01)
        return res
