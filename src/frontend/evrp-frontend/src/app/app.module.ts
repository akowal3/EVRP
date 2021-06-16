import {NgModule} from '@angular/core';
import {BrowserModule} from '@angular/platform-browser';
import {ReactiveFormsModule} from '@angular/forms';
import {MatSlideToggleModule} from '@angular/material/slide-toggle';
// import {MatCheckboxModule} from '@angular/material/checkbox';
import {AppComponent} from './app.component';
import {MapComponent} from './map/map.component';
import {RoutingRequestComponent} from './routing-request/routing-request.component';
import {HttpClientModule} from '@angular/common/http';

@NgModule({
    declarations: [
        AppComponent,
        MapComponent,
        RoutingRequestComponent
    ],
    imports: [
        BrowserModule,
        ReactiveFormsModule,
        MatSlideToggleModule,
        HttpClientModule,
    ],
    providers: [],
    bootstrap: [AppComponent]
})
export class AppModule {
}
