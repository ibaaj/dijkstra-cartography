#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <sstream>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>

#include <PROJ/proj_api.h>

#include <cairo/cairo.h>


using namespace std;


const double symmetryCenter = 3.5;
const double coefDecrease = 100;


double getIntensity(int count, int nbEdges){
    double x = (double)count/(double)nbEdges;
    return (double) 1.0 + ((double) (-1.0) / ( 1.0 +exp(-coefDecrease*x+symmetryCenter) ) );
}



struct edge {
    double x0, y0, x1, y1;

    friend bool operator<(edge a, edge b) {
        if(a.x0 < b.x0
           || (a.x0==b.x0 && a.x1 < b.x1)
           || (a.x0==b.x0 && a.x1==b.x1 && a.y0 < b.y0)
           || (a.x0==b.x0 && a.x1==b.x1 && a.y0 == b.y0 && a.y1 < b.y1))
            return true;
        else
            return false;
    }
    friend bool operator>(edge a, edge b) {
        if(a.x0 > b.x0
           || (a.x0==b.x0 && a.x1 > b.x1)
           || (a.x0==b.x0 && a.x1==b.x1 && a.y0 > b.y0)
           || (a.x0==b.x0 && a.x1==b.x1 && a.y0 == b.y0 && a.y1 > b.y1))
            return true;
        else
            return false;
    }
    friend class boost::serialization::access;
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & x0;
        ar & y0;
        ar & x1;
        ar & y1;

    }
};



template <typename T1, typename T2>
struct less_second {
    typedef pair<T1, T2> type;
    bool operator ()(type const& a, type const& b) const {
        return a.second > b.second;
    }
};





int main() {

    projPJ pj_laea,
            pj_wsg84 = pj_init_plus("+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs "),
            pj_merc = pj_init_plus("+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext  +no_defs");

    std::ifstream f1("edges.map", std::ios::binary);

    std::ostringstream projlaeaoss;
    projlaeaoss << "+proj=laea ";

    std::map<edge, int> db;

    double minLatWSG84, maxLatWSG84,minLonWSG84, maxLonWSG84;
    double minLatLaea, maxLatLaea, minLonLaea, maxLonLaea;
    double minLatMerc, maxLatMerc, minLonMerc, maxLonMerc;
    double tmpy1,tmpy0, tmpx1, tmpx0;

    cairo_surface_t *surface;
    cairo_t *cr;

    double scale = 500.0;

    boost::archive::binary_iarchive iarch(f1);
    iarch >> db;

    vector<pair<edge, int>> mapcopy(db.begin(), db.end());
    sort(mapcopy.begin(), mapcopy.end(), less_second<edge, int>());
    std::vector<pair<edge,int>>::iterator it = mapcopy.begin();


    minLatWSG84 = ((it->first.x0 < it->first.x1) ? it->first.x0 : it->first.x1);
    maxLatWSG84 = ((it->first.x0 > it->first.x1) ? it->first.x0 : it->first.x1);
    minLonWSG84 = ((it->first.y0 < it->first.y1) ? it->first.y0 : it->first.y1);
    maxLonWSG84 = ((it->first.y0 > it->first.y1) ? it->first.y0 : it->first.y1);

    ++it;

    for(; it != mapcopy.end(); ++it)
    {

        if(minLatWSG84 > it->first.x0)
            minLatWSG84 = it->first.x0;

        if(minLatWSG84 > it->first.x1)
            minLatWSG84 = it->first.x1;

        if(maxLatWSG84 < it->first.x0)
            maxLatWSG84 = it->first.x0;
        if(maxLatWSG84 < it->first.x1)
            maxLatWSG84 = it->first.x1;

        if(minLonWSG84 > it->first.y0)
            minLonWSG84 = it->first.y0;
        if(minLonWSG84 > it->first.y1)
        minLonWSG84 = it->first.y1;

        if(maxLonWSG84 < it->first.y0)
            maxLonWSG84 = it->first.y0;
        if(maxLonWSG84 < it->first.y1)
            maxLonWSG84 = it->first.y1;



    }
    std::cout << "WSG84 :" << std::endl;
    std::cout << "minLat: " << minLatWSG84 << std::endl;
    std::cout << "minLon: " << minLonWSG84 << std::endl;
    std::cout << "maxLat: " << maxLatWSG84 << std::endl;
    std::cout << "maxLon: " << maxLonWSG84 << std::endl;

    /*projlaeaoss << " +lat_0=" << ((maxLatWSG84 + minLatWSG84)/2);
    projlaeaoss << " +lon_0=" << ((maxLonWSG84 + minLonWSG84)/2);
    projlaeaoss << " +x_0=" << minLatWSG84;
    projlaeaoss << " +y_0=" << minLonWSG84;
    projlaeaoss << " +ellps=WGS84 +datum=WGS84 +units=m +no_defs";

    std::cout << projlaeaoss.str() << "init" << std::endl;

    pj_laea = pj_init_plus(projlaeaoss.str().c_str());


    it = mapcopy.begin();

    std::cout << "pointer pb ?" << std::endl;

    tmpx0 = it->first.x0*DEG_TO_RAD;
    tmpx1 = it->first.x1*DEG_TO_RAD;
    tmpy0 = it->first.y0*DEG_TO_RAD;
    tmpy1 = it->first.y1*DEG_TO_RAD;

    std::cout << "pointer ok" << std::endl;

    std::cout << "old x0" << tmpx0 << std::endl;
    pj_transform(pj_wsg84, pj_laea, 1, 1, &tmpx0, &tmpy0, NULL );
    pj_transform(pj_wsg84, pj_laea, 1, 1, &tmpx1, &tmpy1, NULL );

    std::cout << "proj4 ok" << std::endl;

    std::cout << "new x0" << tmpx0 << std::endl;

    it->first.x0 = tmpx0;
    it->first.y0 = tmpy0;
    it->first.x1 = tmpx1;
    it->first.y1 = tmpy1;


    minLatLaea = ((tmpx0 < tmpx1) ? tmpx0 : tmpx1);
    maxLatLaea = ((tmpx0 > tmpx1) ? tmpx0 : tmpx1);
    minLonLaea = ((tmpy0 < tmpy1) ? tmpy0 : tmpy1);
    maxLonLaea = ((tmpy0 > tmpy1) ? tmpy0 : tmpy1);

    ++it;

    for(; it != mapcopy.end(); ++it)
    {
        tmpx0 = it->first.x0*DEG_TO_RAD;
        tmpx1 = it->first.x1*DEG_TO_RAD;
        tmpy0 = it->first.y0*DEG_TO_RAD;
        tmpy1 = it->first.y1*DEG_TO_RAD;



        pj_transform(pj_wsg84, pj_laea, 1, 1, &tmpx0, &tmpy0, NULL );
        pj_transform(pj_wsg84, pj_laea, 1, 1, &tmpx1, &tmpy1, NULL );

        it->first.x0 = tmpx0;
        it->first.y0 = tmpy0;
        it->first.x1 = tmpx1;
        it->first.y1 = tmpy1;

        if(minLatLaea > tmpx0)
            minLatLaea = tmpx0;

        if(minLatLaea > tmpx1)
            minLatLaea = tmpx1;

        if(maxLatLaea < tmpx0)
            maxLatLaea = tmpx0;
        if(maxLatLaea < tmpx1)
            maxLatLaea = tmpx1;

        if(minLonLaea > tmpy0)
            minLonLaea = tmpy0;
        if(minLonLaea > tmpy1)
            minLonLaea = tmpy1;

        if(maxLonLaea < tmpy0)
            maxLonLaea = tmpy0;
        if(maxLonLaea < tmpy1)
            maxLonLaea = tmpy1;

    }

    std::cout << "LAEA :" << std::endl;
    std::cout << "minLat: " << minLatLaea << std::endl;
    std::cout << "minLon: " << minLonLaea << std::endl;
    std::cout << "maxLat: " << maxLatLaea << std::endl;
    std::cout << "maxLon: " << maxLonLaea << std::endl;

    */
    it = mapcopy.begin();



    tmpx0 = it->first.x0*DEG_TO_RAD;
    tmpx1 = it->first.x1*DEG_TO_RAD;
    tmpy0 = it->first.y0*DEG_TO_RAD;
    tmpy1 = it->first.y1*DEG_TO_RAD;

    pj_transform(pj_wsg84, pj_merc, 1, 1, &tmpx0, &tmpy0,NULL );
    pj_transform(pj_wsg84, pj_merc, 1, 1, &tmpx1, &tmpy1, NULL );

    it->first.x0 = tmpx0;
    it->first.y0 = tmpy0;
    it->first.x1 = tmpx1;
    it->first.y1 = tmpy1;


    minLatMerc = ((tmpx0 < tmpx1) ? tmpx0 : tmpx1);
    maxLatMerc = ((tmpx0 > tmpx1) ? tmpx0 : tmpx1);
    minLonMerc = ((tmpy0 < tmpy1) ? tmpy0 : tmpy1);
    maxLonMerc = ((tmpy0 > tmpy1) ? tmpy0 : tmpy1);




    ++it;

    for(; it != mapcopy.end(); ++it)
    {

        tmpx0 = it->first.x0*DEG_TO_RAD;
        tmpx1 = it->first.x1*DEG_TO_RAD;
        tmpy0 = it->first.y0*DEG_TO_RAD;
        tmpy1 = it->first.y1*DEG_TO_RAD;



        pj_transform(pj_wsg84, pj_merc, 1, 1, &tmpx0, &tmpy0, NULL );
        pj_transform(pj_wsg84, pj_merc, 1, 1, &tmpx1, &tmpy1, NULL );

        it->first.x0 = tmpx0;
        it->first.y0 = tmpy0;
        it->first.x1 = tmpx1;
        it->first.y1 = tmpy1;

        if(minLatMerc > tmpx0)
            minLatMerc = tmpx0;

        if(minLatMerc > tmpx1)
            minLatMerc = tmpx1;

        if(maxLatMerc < tmpx0)
            maxLatMerc = tmpx0;
        if(maxLatMerc < tmpx1)
            maxLatMerc = tmpx1;

        if(minLonMerc > tmpy0)
            minLonMerc = tmpy0;
        if(minLonMerc > tmpy1)
            minLonMerc = tmpy1;

        if(maxLonMerc < tmpy0)
            maxLonMerc = tmpy0;
        if(maxLonMerc < tmpy1)
            maxLonMerc = tmpy1;

    }

    std::cout << "Merc :" << std::endl;
    std::cout << "minLat: " << minLatMerc << std::endl;
    std::cout << "minLon: " << minLonMerc << std::endl;
    std::cout << "maxLat: " << maxLatMerc << std::endl;
    std::cout << "maxLon: " << maxLonMerc << std::endl;






    int nbEdges = mapcopy.size(),
            count = 0,
            maxLineWidth = 40,
            maxColorGrey = 140,
            lineWidth = 130,
            minX = minLatMerc,
            maxX = maxLatMerc,
            minY = minLonMerc,
            maxY = maxLonMerc,
            offsetLineWidth = 20,
            cutOff = 6000000;

            int h = (maxX - minX)/scale,
                w = (maxY - minY)/scale;

    double intensity,colorUsed;

    surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24,w,h);
    cr = cairo_create (surface);

    int x0,y0,x1,y1;

    cairo_new_path (cr);
    cairo_rectangle(cr, 0, 0, w, h);
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_fill(cr);

    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

    for( it = mapcopy.begin(); it != mapcopy.end(); ++it){

       /* if(count > cutOff){
            break;
        }*/

        intensity = getIntensity(count,nbEdges);


        y0 = (it->first.x0 - minX)/scale;
        x0 = (it->first.y0 - minY)/scale;
        y1 = (it->first.x1 - minX)/scale;
        x1 = (it->first.y1 - minY)/scale;



        //colorUsed = 1.0-intensity-0.8;
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
