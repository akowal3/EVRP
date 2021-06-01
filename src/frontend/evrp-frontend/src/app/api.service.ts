import {Injectable} from '@angular/core';
import {HttpClient} from '@angular/common/http';
import {Observable} from 'rxjs';
import {EVRoute} from "./global.service";

@Injectable({
    providedIn: 'root'
})
export class ApiService {


    baseURL: string = "http://localhost";
    port: number = 5050

    constructor(private http: HttpClient) {
    }

    postEVRequest(req: JSON): Observable<any> {
        console.log(req)

        const headers = {
            'content-type': 'application/json',
        };
        const body = JSON.stringify(req);
        return this.http.post<EVRoute>(this.baseURL + ':' + this.port + '/evroute/', body, {'headers': headers})
    }
}
