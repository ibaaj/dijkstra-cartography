"use strict";

var fs = require('fs');
var util = require('util');

var request = require('sync-request');
var OSMStream = require('node-osm-stream');

var proj4 = require('proj4');
var _ = require('lodash');

var parser = OSMStream();

var homeLat = XXXXXX,
    homeLon = XXXXXXX;

var minX,
    minY,
    maxX,
    maxY;

var diffx, diffy;

var nodeCount = 0;

proj4.defs("EPSG:27571", "+proj=lcc +lat_1=49.50000000000001 +lat_0=49.50000000000001" +
    "+lon_0=0 +k_0=0.999877341 +x_0=600000 +y_0=1200000 +a=6378249.2 +b=6356515" +
    "+towgs84=-168,-60,320,0,0,0,0 +pm=paris +units=m +no_defs");


var limitnode = 0;

var tableVecteurs = {},
    stats = {};

try {
    fs.accessSync("stats.json", fs.F_OK);
    stats = JSON.parse(fs.readFileSync('stats.json'));
    limitnode = stats.nodeCount;
    minX = stats.minX;
    minY = stats.minY;
    maxX = stats.maxX;
    maxY = stats.maxY;
    try {
        fs.accessSync("data.json", fs.F_OK);
        tableVecteurs = JSON.parse(fs.readFileSync('data.json'));
        console.log('loaded data.json');
    } catch (e) {
        console.log('data.json not found');
    }
} catch (e) {
    console.log('stats.json not found');
}


var readstream = fs.createReadStream('./PARIS.osm');

readstream.pipe(parser);



parser.on('node', function(node, callback) {

    nodeCount++;
    if (nodeCount > limitnode) {
        var url = 'http://localhost:8989/route?point='
              + node.lat + ',' + node.lon
              + '&point=' + homeLat + ',' + homeLon
              + '&instructions=false&points_encoded=false';

        console.log("node n°:" + nodeCount + " | " + url);

        try {
            var res = request('GET', url);
        } catch (e) {
            nodeCount--;
            fs.writeFileSync('stats.json', JSON.stringify(stats));
            fs.writeFileSync('data.json', JSON.stringify(tableVecteurs));
            console.log('data saved.');
        }

        if (res.statusCode != 400) {
            var edges = JSON.parse(res.getBody()).paths[0].points.coordinates;
            var projEdges = [];
            for (var i in edges)
                projEdges.push(proj4("EPSG:27571", [edges[i][0], edges[i][1]]));

            var minx93 = _.minBy(projEdges, function(o) {
                    return o[0];
                })[0],
                miny93 = _.minBy(projEdges, function(o) {
                    return o[1];
                })[1],
                maxx93 = _.maxBy(projEdges, function(o) {
                    return o[0];
                })[0],
                maxy93 = _.maxBy(projEdges, function(o) {
                    return o[1];
                })[1];

            if (typeof minX === 'undefined' || minX > minx93)
                minX = minx93;
            if (typeof minY === 'undefined' || minY > miny93)
                minY = miny93;
            if (typeof maxX === 'undefined' || maxX < maxx93)
                maxX = maxx93;
            if (typeof maxY === 'undefined' || maxY < maxy93)
                maxY = maxy93;

            var tmpVect = projEdges[0];

            for (var j = 1; j < projEdges.length; j++) {

                if (tableVecteurs.hasOwnProperty(tmpVect[0])) {

                    if (tableVecteurs[tmpVect[0]].hasOwnProperty(tmpVect[1])) {

                        if (tableVecteurs[tmpVect[0]][tmpVect[1]].hasOwnProperty(projEdges[j][0])) {

                            if (tableVecteurs[tmpVect[0]][tmpVect[1]][projEdges[j][0]].hasOwnProperty(projEdges[j][1])) {
                                tableVecteurs[tmpVect[0]][tmpVect[1]][projEdges[j][0]][projEdges[j][1]]++;

                            } else {
                                tableVecteurs[tmpVect[0]][tmpVect[1]][projEdges[j][0]][projEdges[j][1]] = 1;
                            }

                        } else {
                            tableVecteurs[tmpVect[0]][tmpVect[1]][projEdges[j][0]] = {};
                            tableVecteurs[tmpVect[0]][tmpVect[1]][projEdges[j][0]][projEdges[j][1]] = 1;
                        }

                    } else {
                        tableVecteurs[tmpVect[0]][tmpVect[1]] = {};
                        tableVecteurs[tmpVect[0]][tmpVect[1]][projEdges[j][0]] = {};
                        tableVecteurs[tmpVect[0]][tmpVect[1]][projEdges[j][0]][projEdges[j][1]] = 1;
                    }
                } else {
                    tableVecteurs[tmpVect[0]] = {};
                    tableVecteurs[tmpVect[0]][tmpVect[1]] = {};
                    tableVecteurs[tmpVect[0]][tmpVect[1]][projEdges[j][0]] = {};
                    tableVecteurs[tmpVect[0]][tmpVect[1]][projEdges[j][0]][projEdges[j][1]] = 1;
                }

                tmpVect = projEdges[j];

            }


            diffx = maxX - minX;
            diffy = maxY - minY;

            stats = {
                minX,
                minY,
                maxX,
                maxY,
                diffx,
                diffy,
                nodeCount
            };

        }

    }


});
readstream.on('open', function() {
    console.log('opening file....');
});


readstream.on('end', function() {
    fs.writeFileSync('stats.json', JSON.stringify(stats));
    fs.writeFileSync('data.json', JSON.stringify(tableVecteurs));
    console.log("end, data saved.");
    process.exit(0);

});
