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
    useChargerIDs!: boolean;
    car: string[] = ['Custom', 'Tesla Model 3', 'Renault Zoe']

    socValidators = [Validators.required, Validators.min(5.0), Validators.max(100.0)]
    coordinatesValidator = [Validators.pattern('[0-9]?[0-9][.][0-9]+,[ ]?[0-9]?[0-9][.][0-9]+')]

    constructor(private api: ApiService, private g: GlobalService) {
    }

    ngOnInit() {
        this.evRequest = new FormGroup({
            source_coordinates: new FormControl('52.255700, 20.940975', this.coordinatesValidator),
            source_id: new FormControl(0),
            destination_coordinates: new FormControl('52.255700, 20.940975', this.coordinatesValidator),
            destination_id: new FormControl(0),
            startSoc: new FormControl(50.0, this.socValidators),
            socMin: new FormControl(10, this.socValidators),
            endSoc: new FormControl(40.0, this.socValidators),
            socMax: new FormControl(90, this.socValidators),
            car: new FormControl(1),
            charger_overhead: new FormControl(15, [Validators.required, Validators.min(0), Validators.max(120)])
        });
        this.useChargerIDs = false;
    }

    onSubmit(form: FormGroup) {

        let req = this.buildRequest()

        this.api.postEVRequest(req).subscribe(data => {
            this.g.passRequest(data);
        })

    }

    selectCar(e: any) {
        this.evRequest.setValue(e.target.value, {
            onlySelf: true
        })
    }

    coordinatesToFloatArray(coords: string[]) {
        return [parseFloat(coords[0].trim()), parseFloat(coords[1].trim())]
    }

    buildRequest() {
        let request: any = {}

        request.source = {}
        request.destination = {}
        request.car = {}
        request.config = {}

        request.source.chargerID = parseInt(this.evRequest.value.source_id)

        if (!this.useChargerIDs) {
            let coords = this.evRequest.value.source_coordinates.split(',')
            request.source.coordinates = this.coordinatesToFloatArray(coords)
        } else {
            request.source.coordinates = []
        }

        request.destination.chargerID = parseInt(this.evRequest.value.destination_id)
        if (!this.useChargerIDs) {
            let coords = this.evRequest.value.destination_coordinates.split(',')
            request.destination.coordinates = this.coordinatesToFloatArray(coords)
        } else {
            request.destination.coordinates = []
        }

        request.config.socMin = this.evRequest.value.socMin / 100.0
        request.config.socMax = this.evRequest.value.socMax / 100.0
        request.config.startSoc = this.evRequest.value.startSoc / 100.0
        request.config.endSoc = this.evRequest.value.endSoc / 100.0
        request.config.charger_overhead_in_minutes = this.evRequest.value.charger_overhead

        request.car.carID = this.evRequest.value.car
        request.car.custom = {}


        return <JSON>request
    }

}
