import {Component, OnInit} from '@angular/core';
import {ActivatedRoute, ParamMap} from '@angular/router';
import {Location} from '@angular/common'
import 'rxjs/add/operator/switchMap';

import {Target} from './target';
import {TargetService} from './target.service';

@Component({
    selector: 'target-detail',
    templateUrl: './target-detail.component.html',
    styleUrls: ['./target-detail.component.css'],
})
export class TargetDetailComponent implements OnInit {
    target: Target;
    
    constructor(
        private targetService: TargetService,
        private route: ActivatedRoute,
        private location: Location
    ) {}

    ngOnInit(): void {
        this.route.paramMap
            .switchMap((params: ParamMap) => this.targetService.getTarget(+params.get('id')))
            .subscribe(target => this.target = target);
    }

    saveAndGoBack(): void {
        this.targetService.update(this.target).then(() => this.goBack());
    }

    goBack(): void {
        this.location.back();
    }    
    
}

