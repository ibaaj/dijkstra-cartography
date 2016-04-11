# Railway

![../results/SD/railway.png](../results/SD/railway.png)



## Cook Recipe


- [Downloaded latest weekly planet PBF File](http://planet.openstreetmap.org/) (31GB)
- Installed [Osmfilter](http://wiki.openstreetmap.org/wiki/Osmfilter) and [Osmconvert](http://wiki.openstreetmap.org/wiki/Osmconvert)
- Ran this :
```bash
 ./osmfilter -v planet-latest.o5m --drop-nodes="railway=rail" --drop-relations="railway=rail" --keep="railway=rail" -o=planet-railway.o5m
 ./osmconvert planet-railway.o5m -o=planet-railway.osm
 ./osmconvert planet-railway.osm.pbf -B=eurasia.poly -o=eurasia-railway.osm
  ./osmconvert eurasia-railway.osm -o=eurasia-railway.osm.pbf
```
(see eurasia.poly inside ./misc/ ).

- Installed [Docker](https://docker.com) on my machine, because it's a hell to compile [OSRM](https://github.com/Project-OSRM/osrm-backend) on OSX
- I successed in compiling OSRM on a Debian machine but not with Docker [...] so I downloaded from it osrm-contract, osrm-datastore, osrm-extract, osrm-routed....
- [Downloaded and modified this file](https://flexnst.ru/2015/11/20/osrm-railway-profile/) to get it working with the last version of OSRM... (see ./misc).
- Launched my docker machine (see Dockerfile in ./misc) ```docker run -d -p 5000:5000 owner/name```
- compile & execute read/main.cpp
```
g++ -o read -std=c++11 -lprotobuf-lite -losmpbf -lz -lcurl -lboost_serialization main.cpp
```

- compile & execute draw/main.cpp
```g++  -std=c++11 -framework PROJ -lboost_serialization -I/usr/local/include/cairo -L/usr/local/lib/ -lcairo main.cpp```


## Settings

- URL to deal with OSRM : line 21 in read/main.cpp

## About

I don't excepted the result (plot eurasia.poly [...])
