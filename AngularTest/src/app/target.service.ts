import {Injectable} from '@angular/core';
import {Headers, Http} from "@angular/http";
import 'rxjs/add/operator/toPromise';

import {Target} from './target';

@Injectable()
export class TargetService {

    private targetsUrl = 'api/targets';
    private headers = new Headers({'Content-Type': 'application/json'});

    constructor(private http: Http) {}

    getTargets(): Promise<Target[]> {
        return this.http.get(this.targetsUrl).toPromise().then(response => response.json().data as Target[]).catch(this.handleError);
    }

    getTarget(n: number): Promise<Target> {
        const url = `${this.targetsUrl}/${n}`;
        return this.http
            .get(url)
            .toPromise()
            .then(reponse => reponse.json().data as Target)
            .catch(this.handleError);
    }

    handleError(error: any): Promise<any> {
        console.error('Error fetching targets', error);

        return Promise.reject(error.message || error);
    }

    update(target: Target): Promise<Target> {
        const url = `${this.targetsUrl}/${target.id}`;
        return this.http
            .put(url, JSON.stringify(target), {headers: this.headers})
            .toPromise()
            .then(() => target)
            .catch(this.handleError);
    }

    create(targetName: string): Promise<Target> {
        console.warn(targetName);
        return this.http
            .post(this.targetsUrl, JSON.stringify({name: targetName}), {headers: this.headers})
            .toPromise()
            .then(res => res.json().data as Target)
            .catch(this.handleError);
    }

    delete(targetId: number): Promise<void> {
        const url = `${this.targetsUrl}/${targetId}`;
        return this.http.delete(url, {headers: this.headers})
            .toPromise()
            .then(() => null)
            .catch(this.handleError);
    }
}
