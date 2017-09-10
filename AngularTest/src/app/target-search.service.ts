import { Injectable } from '@angular/core';
import { Http }       from '@angular/http';
 
import { Observable }     from 'rxjs/Observable';
import 'rxjs/add/operator/map';
 
import { Target }           from './target';

@Injectable()
export class TargetSearchService {
    
    constructor(private http: Http) {}
    
    search(term: string): Observable<Target[]> {
        return this.http.get(`api/targets/?name=${term}`)
        .map(response => response.json().data as Target[]);
    }
}