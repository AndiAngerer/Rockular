import {Component, OnInit} from '@angular/core';
import {Router} from '@angular/router';

import {Target} from './target';
import {TargetService} from "./target.service";


@Component({
    selector: 'targets',
    templateUrl: './targets.component.html',
    styleUrls: ['./targets.component.css'],
})
export class TargetsComponent implements OnInit {
    targets: Target[];
    selectedTarget: Target;

    constructor(private targetService: TargetService, private router: Router) {}

    ngOnInit(): void {
        this.getTargets();
    }

    getTargets(): void {
        this.targetService.getTargets().then(targets => this.targets = targets);
    }

    onSelect(target: Target): void {
        this.selectedTarget = target;
    }
    
    gotoSelectedDetail(): void {
        this.router.navigate(['/detail', this.selectedTarget.id]);
    }
    
    gotoDetail(target : Target): void {
        this.router.navigate(['/detail', target.id]);
    }
    
    add(targetName: string): void {
        targetName = targetName.trim();
        if(!targetName) { return; }
        console.warn(targetName);
        this.targetService.create(targetName)
            .then(target => {
                this.targets.push(target);
                this.selectedTarget = null;
            })
    }
    
    delete(target: Target): void {
        this.targetService
        .delete(target.id)
        .then(() => {
            this.targets = this.targets.filter(t => t !== target);
            if (this.selectedTarget === target) {this.selectedTarget = null;}
        });
    }
}