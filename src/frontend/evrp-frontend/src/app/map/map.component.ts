import {AfterViewInit, Component} from '@angular/core';
import * as L from 'leaflet';
import 'leaflet-routing-machine';
import {EVRoute, GlobalService} from "../global.service";

@Component({
    selector: 'app-map',
    templateUrl: './map.component.html',
    styleUrls: ['./map.component.css']
})
export class MapComponent implements AfterViewInit {
    private m: any;
    private routing_control: L.Routing.Control | undefined;
    private readonly router: L.Routing.IRouter;

    private initMap(): void {
        this.m = L.map('map', {
            center: [51.9194, 19.1451],
            zoom: 6.5
        });

        L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
            maxZoom: 18,
            minZoom: 3,
            attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
        }).addTo(this.m);

    }

    constructor(private g: GlobalService) {
        g.evRequest$.subscribe(data => {
            this.processResponse(data)
        })

        this.router = L.Routing.osrmv1({
            serviceUrl: 'http://localhost:5000/route/v1',
            profile: 'car',
        })

    }

    ngAfterViewInit(): void {
        this.initMap();
    }

    processResponse(data: EVRoute) {
        console.log("from map")
        console.log(data)

        let waypoints = data.osrm.waypoints.map((p) => {
            return L.latLng(p.location[1], p.location[0])
        });

        if (this.routing_control != undefined) {
            this.routing_control.remove()
        }

        this.routing_control = L.Routing.control({
            router: this.router,
            waypoints: waypoints,
            plan: new L.Routing.Plan(waypoints, {
                createMarker: function (i: number, start: L.Routing.Waypoint, n: number) {

                    if (i == 0) return new L.Marker(start.latLng, {opacity: 0.0, interactive: false});
                    return new L.Marker(start.latLng);
                }
            }),
            showAlternatives: false,
            show: false, // hide the default direction box as it is unnecessary
            autoRoute: true,
            fitSelectedRoutes: false,
        });

        this.routing_control.addTo(this.m);


        // let t = L.latLng([51.22, 11.22])
        //
        // let t = new L.Routing.Waypoint(L.latLng([51.22, 11.22]), 'test', {});

    }
}