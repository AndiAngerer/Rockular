import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';

import { DashboardComponent } from './dashboard.component';
import { TargetsComponent } from './targets.component';
import { TargetDetailComponent } from './target-detail.component';

const routes : Routes = [
    {
        path: '',
        redirectTo: '/dashboard',
        pathMatch: 'full'
    },
    {
        path: 'targets',
        component : TargetsComponent
    },
    {
        path: 'dashboard',
        component : DashboardComponent
    },
    {
        path: 'detail/:id',
        component: TargetDetailComponent
    }
];

@NgModule({
    imports: [RouterModule.forRoot(routes)],
    exports: [RouterModule]
})
export class AppRoutingModule {}