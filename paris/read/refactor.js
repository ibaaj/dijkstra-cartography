var fs = require('fs');

var data = JSON.parse(fs.readFileSync('data.json'));

var x0,y0,x1,y1,hits,entity,sortedData = [];


for(x0 in data)
  for(y0 in data[x0])
    for(x1 in data[x0][y0])
      for(y1 in data[x0][y0][x1]){
        entity = { 'x0':x0, 'y0':y0, 'x1':x1, 'y1':y1, hits: data[x0][y0][x1][y1] };
        sortedData.push(entity);
      }

sortedData.sort(function(a, b) { return - (a.hits - b.hits)});

fs.writeFileSync("data-refactored.json", JSON.stringify(sortedData, null, 0));

process.exit(0);
