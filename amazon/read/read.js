
var fs = require('fs');
var _ = require('lodash');


var OSMStream = require('node-osm-stream');
var parser = OSMStream();

var proj4 = require('proj4');

var minX = undefined,
    minY = undefined,
    maxX = undefined,
    maxY = undefined;

var canvas, ctx;

var diffx, diffy;

var edges = [];

var nodeCount = 0;

var maxLineWidth = 130;
var maxColorGrey = 220;


proj4.defs("ESRI:102032", "+proj=eqdc +lat_0=-32 +lon_0=-60"+
"+lat_1=-5 +lat_2=-42 +x_0=0 +y_0=0 +ellps=aust_SA +units=m +no_defs");


var tributaries = JSON.parse(fs.readFileSync('tributaries.json'));

// open a local .osm filestream
var readstream = fs.createReadStream('amazon.osm');

readstream.pipe(parser);

var relationsMap = {},
    waysMap = {},
    nodesMap = {};

readstream.on('open', function(){
  console.log('opened file...');
})

parser.on('node', function(node, callback){
    nodesMap[node.id] = {lat: node.lat, lon: node.lon };
});
parser.on('way', function(way, callback){
    waysMap[way.id] = way.nodes;
});
parser.on('relation', function(relation, callback){
  relationsMap[relation.id] = relation.members;

});

readstream.on('end', function(){
  console.log("now analyzing...");
   analyze();
});

var analyze = function() {
  var tributarie, relation, wayObj, way, node, nodeObj,
  tmpProj, tmpProj2;

  for(tributarie in tributaries){

    relation = relationsMap[tributaries[tributarie][0]];

        for(wayObj in relation){

          way = waysMap[relation[wayObj].ref];


              for(node in way){

                nodeCount++;

                node = nodesMap[way[node]];

                if(typeof tmpProj === 'undefined'){
                  tmpProj = proj4("ESRI:102032", [node.lat, node.lon]);

                  if (typeof minX === 'undefined' || minX > tmpProj[0])
                      minX = tmpProj[0];
                  if (typeof minY === 'undefined' || minY > tmpProj[1])
                      minY = tmpProj[1];
                  if (typeof maxX === 'undefined' || maxX < tmpProj[0])
                      maxX = tmpProj[0];
                  if (typeof maxY === 'undefined' || maxY < tmpProj[1])
                      maxY = tmpProj[1];

                  continue;
                }


                tmpProj2 = proj4("ESRI:102032", [node.lat, node.lon]);
                edge = { x0: tmpProj[0],
                          y0: tmpProj[1],
                          x1: tmpProj2[0],
                          y1: tmpProj2[1],
                          hits: tributaries[tributarie][1] };

              if (minX > tmpProj2[0])
                  minX = tmpProj2[0];
              if (minY > tmpProj2[1])
                  minY = tmpProj2[1];
              if (maxX < tmpProj2[0])
                  maxX = tmpProj2[0];
              if (maxY < tmpProj2[1])
                  maxY = tmpProj2[1];

                edges.push(edge);
                tmpProj = tmpProj2;


              }
              tmpProj = undefined;
          }
    }

    console.log("We have " + edges.length + " edges.");

    var diffx = maxX - minX,
        diffy = maxY - minY;

        var stats = {
            minX,
            minY,
            maxX,
            maxY,
            diffx,
            diffy
          };

        fs.writeFileSync('stats.json', JSON.stringify(stats));
        fs.writeFileSync('data.json', JSON.stringify(edges,null,0));

}
