#!/bin/sh

osmosis \
  --rb south-america-latest.osm.pbf \
  --tf accept-relations waterway=*\
  --used-way \
  --used-node \
  outPipe.0=RW \
  \
  --rb south-america-latest.osm.pbf \
  --tf accept-relations natural=water \
  --used-way \
  --used-node \
  outPipe.0=RN \
  \
  --merge \
  	inPipe.0=RW \
  	inPipe.1=RN \
  	outPipe.0=R \
  \
  --rb south-america-latest.osm.pbf \
  --tf accept-ways waterway=* \
  --used-node \
   outPipe.0=WW \
  \
  --rb south-america-latest.osm.pbf \
  --tf accept-ways natural=water \
  --used-node \
   outPipe.0=WM \
  \
  --merge \
  	inPipe.0=WW \
  	inPipe.1=WM \
  	outPipe.0=W \
  --merge \
  	inPipe.0=R \
  	inPipe.1=W \
  --wx rivers.osm
