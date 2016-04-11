var fs = require('fs');
var _ = require('lodash');

const Graph = require('node-dijkstra')

const route = new Graph();

/*
Mapping the Amazon 2295651
*/
var relations = fs.readFileSync('relations_up.txt').toString().split('\n');

var uniqueTributaries = [];

function parse(riverName, riverFatherName){

  for(var i in relations){
    var river = relations[i];
    if(river.substring(0, riverName.length) === riverName){

      var tributaries = river.substring(river.lastIndexOf('=')+1).split(' ');

      if(tributaries[0] != ''){
        var edges = {};
        for(var j in tributaries){
          edges[tributaries[j]] = 1;

          uniqueTributaries[tributaries[j]] = 1;

          if(tributaries[j] != riverFatherName)
            parse(tributaries[j],riverName);
        }

        if(riverFatherName != '')
          edges[riverFatherName] = 1;

        route.addNode(riverName, edges);
      }

    }

  }

}

parse('2295651', '', 1);

var count = 1,
    tributarie,
    path,
    paths;

uniqueTributaries[2295651]= 1; // Adding the Amazon

for(tributarie in uniqueTributaries){
  paths = route.path('2295651',tributarie);
  console.log(count + "::"+ tributarie +" : " + paths);
  count++;

  for(path in paths){

    uniqueTributaries[parseInt(paths[path])]++;
  }

}

var sortable = [];
for (tributarie in uniqueTributaries)
      sortable.push([tributarie, uniqueTributaries[tributarie]])
sortable.sort(function(a, b) {return - (a[1] - b[1])});

console.log(sortable);

fs.writeFileSync('tributaries.json', JSON.stringify(sortable));
