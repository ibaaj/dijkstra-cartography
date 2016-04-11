# Flights


![../results/SD/flights.png](../results/SD/flights.png)


## Cook Recipe
- [Downloaded and parsed](http://openflights.org/data.html)  : routes.dat and airports.dat from openflights.org 
- Compile & run main.cpp
```
g++ -framework PROJ  -std=c++11 -I/usr/local/include/cairo -L/usr/local/lib/ -lcairo main.cpp
```

## Settings

Change city on line 246 and 294 of main.cpp

## About

I tried to draw [quadratic curve](stackoverflow.com/questions/27404531/drawing-good-looking-if-not-accurate-flight-paths-with-an-html-canvas-bezier-c) instead of line but it was to difficult.
I also didn't choose a good projection (WSG84).