import {Component, OnInit} from '@angular/core';
import {Target} from './target'
import {TargetService} from './target.service'

@Component({
    selector: 'dashboard',
    templateUrl: './dashboard.component.html',
    styleUrls: ['./dashboard.component.css'],
})
export class DashboardComponent implements OnInit {
    
    targets: Target[] = [];
    
    constructor(private targetService : TargetService) {}
    
    ngOnInit(): void {
        this.targetService.getTargets().then(targets => this.targets = targets.slice(0, 3));
    }
}