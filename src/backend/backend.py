import pickle
import sys
from typing import List, Tuple, Dict, Any

import yaml

from charger import Charger
from edge import Edge
from ocm import OCM
from osrm import OSRM
from routing_request import RoutingRequest

sys.path.append("../../cmake-build-debug")
sys.path.append("cmake-build-debug")
from bindings import Node, BuildingEdge, Router, Car


class EvrpBackend:
    def __init__(self):
        self.config = yaml.load(open('config.yaml', 'r'))
        self.chargers: List[Charger] = []
        self.edges: List[Edge] = []
        self.router: Router = None
        self.ocm = OCM(self.config['OSM']['Host'], self.config['OSM']['Key'])
        self.osrm = OSRM(self.config['OSRM']['Host'], self.config['OSRM']['Port'])

    def initialize_router(self) -> None:

        if self.chargers is []:
            raise Exception("Chargers are empty")
        if self.edges is []:
            raise Exception("Edges are empty")
        nodes, edges = self.get_cpp()
        self.router = Router(nodes, edges)

    def to_pickle(self) -> None:
        pickle.dump(self.chargers, open(self.config['ChargersFile'], 'wb'))
        pickle.dump(self.edges, open(self.config['EdgesFile'], 'wb'))

    def get_cpp(self) -> Tuple[List[Node], List[BuildingEdge]]:
        return [n.to_cpp() for n in self.chargers], [e.to_cpp() for e in self.edges]

    def route(self, sourceID: int, destinationID: int, c: Car):
        if not 0 <= sourceID < len(self.chargers):
            raise Exception("Source is not in chargers")
        if not 0 <= destinationID < len(self.chargers):
            raise Exception("Destination is not in chargers")
        if self.router is None:
            raise Exception("Router has not been initialized")

        route = self.router.route(sourceID, destinationID, c)

        print(route)

        charger_ids = [n.ID for n in route.nodes]
        osrm_route = self.get_osrm_route(charger_ids)

        return {
            'code': 'ok',
            'osrm': osrm_route,
            'charge_time': route.charge_time,
            'total_time': route.total_time
        }

    def process_request(self, req: RoutingRequest):
        src = self.process_source(req)
        dst = self.process_destination(req)

        response = self.route(src, dst, req.car())

        self.clear_temporary_nodes(req)

        return response

    def process_source(self, req: RoutingRequest) -> int:
        if req.source_is_charger():
            return req.source_charger_id
        else:
            srcID = self.chargers[-1].internalID + 1
            temp_node_src = Charger.temp_node(srcID, req.source_coordinates)
            edges_from_source = self.osrm.edge_list([temp_node_src], self.chargers)
            cpp_edges_from_source = [e.to_cpp() for e in edges_from_source]
            self.router.add_node(temp_node_src.to_cpp(), cpp_edges_from_source)
            self.chargers.append(temp_node_src)
            return srcID

    def process_destination(self, req: RoutingRequest) -> int:
        if req.destination_is_charger():
            return req.destination_charger_id
        else:
            dstID = self.chargers[-1].internalID + 1
            temp_node_dst = Charger.temp_node(dstID, req.destination_coordinates)
            edges_to_destination = self.osrm.edge_list(self.chargers, [temp_node_dst])
            cpp_edges_to_destination = [e.to_cpp() for e in edges_to_destination]
            self.router.add_node(temp_node_dst.to_cpp(), cpp_edges_to_destination)
            self.chargers.append(temp_node_dst)
            return dstID

    def clear_temporary_nodes(self, req: RoutingRequest) -> None:
        if not req.destination_is_charger():
            c = self.chargers.pop()
            self.router.remove_node_by_id(c.internalID)
        if not req.source_is_charger():
            c = self.chargers.pop()
            self.router.remove_node_by_id(c.internalID)

    def get_osrm_route(self, charger_ids: List[int]) -> Dict[str, Any]:
        waypoints = [self.chargers[i].location() for i in charger_ids]
        print(waypoints)
        return self.osrm.route_result(waypoints)

    @classmethod
    def from_pickle(cls):
        this = cls()
        this.chargers = pickle.load(open(this.config['ChargersFile'], 'rb'))
        this.edges = pickle.load(open(this.config['EdgesFile'], 'rb'))
        this.initialize_router()

        return this

    @classmethod
    def from_services(cls, save_to_file: bool = True):
        this = cls()
        this.chargers = this.ocm.get_chargers_by_country(this.config['OSM']['CountryCode'],
                                                         this.config['OSM']['MaxResults'])
        this.edges = this.osrm.edge_list(this.chargers, this.chargers)
        this.initialize_router()

        if save_to_file:
            this.to_pickle()

        return this

    @staticmethod
    def construct_error(e: Exception) -> Dict[str, Any]:
        return {
            'code': 'fail',
            'message': str(e)
        }
