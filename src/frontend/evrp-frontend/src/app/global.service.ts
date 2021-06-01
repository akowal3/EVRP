import {Injectable} from '@angular/core';
import {Subject} from "rxjs";

@Injectable({
    providedIn: 'root'
})
export class GlobalService {

    evRequest = new Subject<EVRoute>()
    evRequest$ = this.evRequest.asObservable()

    constructor() {
    }

    passRequest(data: EVRoute) {
        this.evRequest.next(data);
    }
}

export interface EVRouteEdge {
    speed_kmh: number;
    distance_km: number;
    consumed_energy: number;
    consumed_soc: number;

}

export interface EVRouteWaypoint {
    latitude: number;
    longitude: number;
    socIn: number;
    socOut: number;
    chargerName: string;
    chargingTime: number;
    chargerID: number;
}

export interface EVRoute {
    waypoints: EVRouteWaypoint[];
    edges: EVRouteEdge[];
    charge_time: number;
    total_time: number;
    code: string;
}


// charge_time: 1586
// code: "ok"
// osrm:
//     code: "Ok"
// routes: Array(1)
// 0: {distance: 150426.7, duration: 8140.5, legs: Array(1), weight: 8191.1, weight_name: "routability"}
// length: 1
// __proto__: Array(0)
// waypoints: Array(2)
// 0:
// distance: 0.233257
// hint: "et5KgP3eSoASAAAAZQAAAEoBAAAAAAAAnfdzQLTTqEFhzYlCAAAAAAkAAAAzAAAApgAAAAAAAAA9BgAAZ-MCARAwDANo4wIBEjAMAwIA7wa_6zDR"
// location: (2) [16.966503, 51.130384]
// name: ""

// total_time: 9767