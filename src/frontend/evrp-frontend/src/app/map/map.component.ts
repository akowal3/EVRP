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
        // L.Map.addInitHook('addHandler', 'cursor', L.Cursor)

        this.m = L.map('map', {
            center: [51.9194, 19.1451],
            zoom: 6.5
        });

        L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
            maxZoom: 18,
            minZoom: 3,
            attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
        }).addTo(this.m);

        this.m.once('dblclick', (e: any) => {
                let latitude = e.latlng.lat
                let longitude = e.latlng.lng
                console.log(e.latlng);
                this.g.mouseLocation.next([latitude, longitude]);
            }
        );

        this.g.mouseLocationRequest$.subscribe((n: any) => {
                this.m.once('dblclick', (e: any) => {
                        let latitude = e.latlng.lat
                        let longitude = e.latlng.lng
                        this.g.mouseLocation.next([latitude, longitude]);
                    }
                );
            }
        )

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


        let waypoints = data.waypoints.map((p) => {
            return L.latLng(p.latitude, p.longitude)
        });

        if (this.routing_control != undefined) {
            this.routing_control.remove()
        }

        this.routing_control = L.Routing.control({
            router: this.router,
            waypoints: waypoints,
            plan: new L.Routing.Plan(waypoints, {
                createMarker: function (i: number, start: L.Routing.Waypoint, n: number) {

                    let current = data.waypoints[i];

                    if (i != 0 && i != n - 1 && current.chargingTime == 0) {
                        // if chosen not to charge on particular charger, then remove the marker for that charger
                        return new L.Marker(start.latLng, {opacity: 0.0, interactive: false});
                    }

                    let socIn = GlobalService.formatPercentage(current.socIn);
                    let socOut = GlobalService.formatPercentage(current.socOut);
                    let chargingTime = GlobalService.formatTime(current.chargingTime);
                    let chargerContent = 'Charge from ' + socIn + ' to ' + socOut + ' in ' + chargingTime + '.';
                    let popupDescription = '';


                    if (i === 0) {
                        popupDescription = 'Source';
                        if (current.chargingTime != 0)
                            popupDescription += '. ' + chargerContent;

                    } else if (i === n - 1)
                        popupDescription = 'Destination';
                    else
                        popupDescription = chargerContent;

                    return new L.Marker(start.latLng).bindPopup(popupDescription).openPopup();
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