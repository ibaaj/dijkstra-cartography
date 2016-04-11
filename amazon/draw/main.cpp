#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>

#include <cairo/cairo.h>

#include "json.hpp"

using json = nlohmann::json;
using namespace std;

const double symmetryCenter = 3.5;
const double coefDecrease = 100;


double getIntensity(int count, int nbEdges){
     double x = (double)count/(double)nbEdges;
     return (double) 1.0 + ((double) (-1.0) / ( 1.0 +exp(-coefDecrease*x+symmetryCenter) ) );
}


int main() {
    double scale = 250.0;

    ifstream t1("data.json");
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
        maxLineWidth = 50,
        maxColorGrey = 140,
        lineWidth = 130,
        minX = (int) stats["minX"],
        maxX = (int) stats["maxX"],
        minY = (int) stats["minY"],
        maxY = (int) stats["maxY"],
        offsetLineWidth = 15;

    int h = (maxX - minX)/scale,
            w = (maxY - minY)/scale;

    double intensity,colorUsed;

    surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24,w,h);
    cr = cairo_create (surface);

    int x0,y0,x1,y1,center_y,tmpY;

    cairo_new_path (cr);
    cairo_rectangle(cr, 0, 0, w, h);
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_fill(cr);

    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

    center_y = h/2;

    for (json::iterator it = data.begin(); it != data.end(); ++it) {


        intensity = getIntensity(count,nbEdges);


        y0 = ((double) it.value()["x0"] - (int) minX)/scale;
        x0 = ((double) it.value()["y0"] - (int) minY)/scale;
        y1 = ((double) it.value()["x1"] - (int) minX)/scale;
        x1 = ((double) it.value()["y1"] - (int) minY)/scale;

        tmpY = y0 - center_y;
        y0 = y0 - 2*tmpY;
        tmpY = y1 - center_y;
        y1 = y1 - 2*tmpY;




       lineWidth = maxLineWidth*intensity +  offsetLineWidth ;
        colorUsed = 0;


        cairo_set_source_rgb(cr, colorUsed,colorUsed,colorUsed);
        cairo_set_line_width (cr, lineWidth);
        cairo_move_to(cr, x0,y0);
        cairo_line_to(cr, x1,y1);
        cairo_stroke(cr);

        count++;


        if(count % 5000 ==0){
            cout << count << "/" << nbEdges << " paths drawn - " << \
                "grey: "<< colorUsed << " | lineWidth: " << lineWidth << endl;
        }


    }

    cairo_surface_write_to_png(surface, "image.png");
    cout << "Done." << endl;

    return 0;
}
