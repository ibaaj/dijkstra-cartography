# Paris

![../results/SD/paris.png](../results/SD/paris.png)

Sadly I started my project with NodeJS :(.

## Cook Recipe

- [OSM File is here](http://download.geofabrik.de/europe/france/ile-de-france.html)
- Extracted Paris.osm with [osmconvert](http://wiki.openstreetmap.org/wiki/Osmconvert) and [this polygon](https://github.com/Zkir/osm2dcm/blob/master/osm2dcm/poly/FR-PARIS.poly).
```
osmconvert ile-de-france-latest.osm.pbf -B=FR-PARIS.poly -o=Paris.pbf
```

- Launched Paris.osm with [Graphhopper](https://github.com/graphhopper/graphhopper) 
- installed sync-request, node-osm-stream, lodash and proj4js via NPM :
```
npm install sync-request node-osm-stream lodash proj4js
```

- then ```node read.js``` (it took me 3 days)
- refactored the data using refactor.js 😓
- compile & run main.cpp 
```bash
g++ -o draw -std=c++11 -I/usr/local/include/cairo -L/usr/local/lib/ -lcairo main.cpp
chmod +x draw
./draw
```

## Settings 

- homeLat & homeLon parameters inside read/read.js
- *.json data filepath inside main.cpp
