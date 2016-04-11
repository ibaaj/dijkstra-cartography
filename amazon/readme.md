# Amazon

![../results/SD/amazon.png](../results/SD/amazon.png)

Sadly I started my project with NodeJS :(.

## Cook Recipe


- [OSM File is here](http://download.geofabrik.de/south-america.html)
- Installed [Osmosis](http://wiki.openstreetmap.org/wiki/Osmosis) and downloaded [this script](https://github.com/InfoAmazonia/rivers-map/blob/master/extract.sh) from @InfoAmazonia
- Modified extract.sh to keep relations's xml nodes (see extract.sh in this repo).
- run ./read/extract.sh and get rivers.osm (~3GB)
- Discovered [this page](http://www.h-renrew.de/h/osm/osmchecks/07_watershed/south-america/hierarchical.html) which shows the waterway relations of south america. Rio Amazonas's ID is 2295651. 
- Parsed ./read/relations_up.txt and get all the tributaries of Amazon river (tributaries.json) with getTributaries.js using Dijkstra algorithm.
- Extracted the edges and vertices sorted by usage using ./read/read.js
- compile & run ./draw/main.cpp in 
```
g++ -o draw -std=c++11 -I/usr/local/include/cairo -L/usr/local/lib/ -lcairo main.cpp 
chmod +x draw
./draw
```

## Settings

Check filepaths in ./draw/main.cpp. 

## Notes
In ./misc/ I tried to recreate everything in C++ but it isn't finished.

To reduce the filesize of rivers.osm, [this answer](https://help.openstreetmap.org/questions/48522/extract-nodes-from-relation-ids-in-a-big-osm-file) helped me a lot, even if it was easy to install osmium - you can find the "reduced rivers.osm file" amazon.osm in this repo.
