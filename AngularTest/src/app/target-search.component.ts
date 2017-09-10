import { Component, OnInit } from '@angular/core';
import { Router }            from '@angular/router';
 
import { Observable }        from 'rxjs/Observable';
import { Subject }           from 'rxjs/Subject';
 
// Observable class extensions
import 'rxjs/add/observable/of';
 
// Observable operators
import 'rxjs/add/operator/catch';
import 'rxjs/add/operator/debounceTime';
import 'rxjs/add/operator/distinctUntilChanged';

import { TargetSearchService } from './target-search.service';
import { Target } from './target';

@Component({
    selector: 'target-search',
    templateUrl: './target-search.component.html',
    styleUrls: [ './target-search.component.css' ],
    providers: [TargetSearchService]
})
export class TargetSearchComponent implements OnInit {
    targets: Observable<Target[]>;
    private searchTerms = new Subject<string>();
    
    constructor(private searchService: TargetSearchService, private router: Router) {}
         
    ngOnInit(): void {
        this.targets = this.searchTerms
            .debounceTime(300)
            .distinctUntilChanged()
            .switchMap(term => 
                term ? this.searchService.search(term) : Observable.of<Target[]>([])
            )
            .catch(error => {
                console.log(error);
                return Observable.of<Target[]>([]);
            });
    }
    
    search(term: string): void {
        this.searchTerms.next(term);
    }
    
    gotoDetail(target: Target): void {
        let link = ['/detail', target.id];
        this.router.navigate(link);
    }
}
