import {InMemoryDbService} from 'angular-in-memory-web-api';

export class InMemoryDataService implements InMemoryDbService {
    createDb() {
        const targets = [
            {id: 1, name: 'Tür'},
            {id: 2, name: 'Fenster'},
            {id: 3, name: 'Außentür'},
            {id: 4, name: 'Andi'},
            {id: 5, name: 'Flasche'},
            {id: 6, name: 'c\'t'}
        ];

        return {targets};
    }

}