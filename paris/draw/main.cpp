#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>


#include <cairo/cairo.h>

#include "json.hpp"

using json = nlohmann::json;
using namespace std;

const double symmetryCenter = 3.5;
const double coefDecrease = 4500;


int jsonToInt(json j){
    string s = j;
    int result;
    return stoi(s.c_str());
}

double getIntensity(int count, int nbEdges){
     double x = (double)count/(double)nbEdges;
     return (double) 1.0 + ((double) (-1.0) / ( 1.0 +exp(-coefDecrease*x+symmetryCenter) ) );
}


int main() {
    double scale = 1;

    ifstream t1("data-refactored.json");
    string dataString( (istreambuf_iterator<char>(t1) ),
                         (istreambuf_iterator<char>()    ) );
    json data = json::parse(dataString);

    ifstream t2("stats.json");
    string statsString( (istreambuf_iterator<char>(t2) ),
                       (istreambuf_iterator<char>()    ) );
    json stats = json::parse(statsString);


    cairo_surface_t *surface;
    cairo_t *cr;


    int nbEdges = data.size(),
        count = 0,
        maxLineWidth = 110,
        lineWidth = 110,
        minX = (int) stats["minX"],
        maxX = (int) stats["maxX"],
        minY = (int) stats["minY"],
        maxY = (int) stats["maxY"],
        offsetLineWidth = 4;

    int w = (maxX - minX)/scale,
            h = (maxY - minY)/scale,
            cutOff = 50000;

    double intensity, colorUsed;

    surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24,w,h);
    cr = cairo_create (surface);

    int x0,y0,x1,y1, center_y, tmpY;

    cairo_new_path (cr);
    cairo_rectangle(cr, 0, 0, w, h);
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_fill(cr);

    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

    center_y = h/2;


    for (json::iterator it = data.begin(); it != data.end(); ++it) {

        intensity = getIntensity(count,nbEdges);


        x0 = (jsonToInt(it.value()["x0"]) - minX)/scale;
        y0 = (jsonToInt(it.value()["y0"]) - minY)/scale;
        x1 = (jsonToInt(it.value()["x1"]) - minX)/scale;
        y1 = (jsonToInt(it.value()["y1"]) - minY)/scale;


        /* Mirroring */
        tmpY = y0 - center_y;
        y0 = y0 - 2*tmpY;
        tmpY = y1 - center_y;
        y1 = y1 - 2*tmpY;


        colorUsed = 0;

        lineWidth = maxLineWidth*intensity +  offsetLineWidth ;


        cairo_set_source_rgb(cr, colorUsed,colorUsed,colorUsed);
        cairo_set_line_width (cr, lineWidth);
        cairo_move_to(cr, x0,y0);
        cairo_line_to(cr, x1,y1);
        cairo_stroke(cr);

        count++;
        if(count % 10000 == 0)
        cout << count << "/" << nbEdges << " paths drawn - " << \
                "grey: "<< colorUsed << " | lineWidth: " << lineWidth << endl;


    }

    cairo_surface_write_to_png(surface, "image.png");

    return 0;
}
