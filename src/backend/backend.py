import pickle
import sys
from typing import List, Tuple

import yaml

from src.backend.graph.charger import Charger
from src.backend.graph.edge import Edge
from src.backend.services.osm import OSM
from src.backend.services.osrm import OSRM

sys.path.append("../../cmake-build-debug")
sys.path.append("cmake-build-debug")
from bindings import Node, BuildingEdge, Router


class EvrpBackend:
    def __init__(self):
        self.config = yaml.load(open('config.yaml', 'r'))
        self.chargers: List[Charger] = []
        self.edges: List[Edge] = []
        self.router: Router = None

    def from_pickle(self) -> None:
        self.chargers = pickle.load(open(self.config['ChargersFile'], 'rb'))
        self.edges = pickle.load(open(self.config['EdgesFile'], 'rb'))

    def to_pickle(self) -> None:
        pickle.dump(self.chargers, open(self.config['ChargersFile'], 'wb'))
        pickle.dump(self.edges, open(self.config['EdgesFile'], 'wb'))

    def from_services(self) -> None:
        osm = OSM(self.config['OSM']['Host'], self.config['OSM']['Key'])
        osrm = OSRM(self.config['OSRM']['Host'], self.config['OSRM']['Port'])
        self.chargers = osm.get_chargers_by_country(self.config['OSM']['CountryCode'], self.config['OSM']['MaxResults'])
        self.edges = osrm.edge_list(self.chargers)
        self.to_pickle()

    def get_cpp(self) -> Tuple[List[Node], List[BuildingEdge]]:
        return [n.to_cpp() for n in self.chargers], [e.to_cpp() for e in self.edges]
