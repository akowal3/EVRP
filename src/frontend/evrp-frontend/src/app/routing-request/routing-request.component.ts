import {Component, OnInit} from '@angular/core';
import {FormControl, FormGroup, Validators} from '@angular/forms';
import {ApiService} from "../api.service";
import {GlobalService} from "../global.service";

@Component({
    selector: 'app-routing-request',
    templateUrl: './routing-request.component.html',
    styleUrls: ['./routing-request.component.css']
})
export class RoutingRequestComponent implements OnInit {
    evRequest!: FormGroup;
    car: string[] = ['Custom', 'Tesla Model 3', 'Renault Zoe']

    locationOption: string[] = ['Select on map', 'Provide coordinates', 'Use chargerID']

    pickCoordinatesFromMap = {
        source: false,
        destination: false
    }

    socValidators = [Validators.required, Validators.min(5.0), Validators.max(100.0)]
    coordinatesValidator = [Validators.pattern('[0-9]?[0-9][.][0-9]+,[ ]?[0-9]?[0-9][.][0-9]+')]


    constructor(private api: ApiService, private g: GlobalService) {
        g.mouseLocation$.subscribe(coordinates => this.getCoordinatesFromMap(coordinates))

    }

    ngOnInit() {
        this.evRequest = new FormGroup({
            source_coordinates: new FormControl(),
            source_id: new FormControl(0),
            destination_coordinates: new FormControl(),
            destination_id: new FormControl(0),
            startSoc: new FormControl(50.0, this.socValidators),
            socMin: new FormControl(10, this.socValidators),
            endSoc: new FormControl(40.0, this.socValidators),
            socMax: new FormControl(90, this.socValidators),
            car: new FormControl(1),
            charger_overhead: new FormControl(15, [Validators.required, Validators.min(0), Validators.max(120)]),
            source_location_choice: new FormControl(),
            destination_location_choice: new FormControl()
        });

        this.evRequest.value.source_coordinates.setValidators(this.coordinatesValidator);
        this.evRequest.value.destination_coordinates.setValidators(this.coordinatesValidator);
        this.evRequest.value.source_location_choice.setValidators([Validators.min(0), Validators.max(3)]);
        this.evRequest.value.destination_location_choice.setValidators([Validators.min(0), Validators.max(3)]);

    }

    onSubmit(form: FormGroup) {

        let req = this.buildRequest(form)

        this.api.postEVRequest(req).subscribe(data => {
            this.g.evRequest.next(data);
        })

    }

    selectSourceLocation() {
        let location_choice = this.evRequest.value.source_location_choice;
        if (location_choice == 0) {
            this.g.mouseLocationRequest.next();
            this.pickCoordinatesFromMap.source = true;
        }
    }

    selectDestinationLocation() {
        let location_choice = this.evRequest.value.destination_location_choice;
        if (location_choice == 0) {
            this.g.mouseLocationRequest.next();
            this.pickCoordinatesFromMap.destination = true;
        }
    }

    getCoordinatesFromMap(coordinates: number[]) {
        let coordinatesString = coordinates[0].toString() + ', ' + coordinates[1].toString();
        if (this.pickCoordinatesFromMap.source) {
            this.evRequest.patchValue({'source_coordinates': coordinatesString});
            this.pickCoordinatesFromMap.source = false;
        } else if (this.pickCoordinatesFromMap.destination) {
            this.evRequest.patchValue({'destination_coordinates': coordinatesString});
            this.pickCoordinatesFromMap.destination = false;
        }
    }

    coordinatesToFloatArray(coords: string[]) {
        return [parseFloat(coords[0].trim()), parseFloat(coords[1].trim())]
    }

    buildRequest(form: FormGroup) {
        let request: any = {}

        request.source = {}
        request.destination = {}
        request.car = {}
        request.config = {}

        request.source.chargerID = parseInt(form.value.source_id)

        if (form.value.source_coordinates) {
            let coords = form.value.source_coordinates.split(',')
            request.source.coordinates = this.coordinatesToFloatArray(coords)
        } else {
            request.source.coordinates = []
        }

        request.destination.chargerID = parseInt(form.value.destination_id)
        if (form.value.destination_coordinates) {
            let coords = form.value.destination_coordinates.split(',')
            request.destination.coordinates = this.coordinatesToFloatArray(coords)
        } else {
            request.destination.coordinates = []
        }

        request.config.socMin = form.value.socMin / 100.0
        request.config.socMax = form.value.socMax / 100.0
        request.config.startSoc = form.value.startSoc / 100.0
        request.config.endSoc = form.value.endSoc / 100.0
        request.config.charger_overhead_in_minutes = form.value.charger_overhead

        request.car.carID = form.value.car
        request.car.custom = {}


        return <JSON>request
    }

}
