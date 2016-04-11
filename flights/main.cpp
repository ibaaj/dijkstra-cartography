#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <limits>
#include <queue>
#include <map>
#include <utility>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <unistd.h>
#include <cmath>

#include <sstream>


#include <PROJ/proj_api.h>
#include <cairo/cairo.h>

using namespace std;



const double symmetryCenter = 3.5;
const double coefDecrease = 200;


double getIntensity(int count, int nbEdges){
    double x = (double)count/(double)nbEdges;
    return (double) 1.0 + ((double) (-1.0) / ( 1.0 +exp(-coefDecrease*x+symmetryCenter) ) );
}


typedef int vertex_t;
typedef double weight_t;

const weight_t max_weight = std::numeric_limits<double>::infinity();

struct neighbor {
    vertex_t target;
    weight_t weight;
    neighbor(vertex_t arg_target, weight_t arg_weight)
            : target(arg_target), weight(arg_weight) { }
};

typedef std::vector<std::vector<neighbor> > adjacency_list_t;
typedef std::pair<weight_t, vertex_t> weight_vertex_pair_t;

void DijkstraComputePaths(vertex_t source,
                          const adjacency_list_t &adjacency_list,
                          std::vector<weight_t> &min_distance,
                          std::vector<vertex_t> &previous)
{
    int n = adjacency_list.size();
    min_distance.clear();
    min_distance.resize(n, max_weight);
    min_distance[source] = 0;
    previous.clear();
    previous.resize(n, -1);
    // we use greater instead of less to turn max-heap into min-heap
    std::priority_queue<weight_vertex_pair_t,
            std::vector<weight_vertex_pair_t>,
            std::greater<weight_vertex_pair_t> > vertex_queue;
    vertex_queue.push(std::make_pair(min_distance[source], source));

    while (!vertex_queue.empty())
    {
        weight_t dist = vertex_queue.top().first;
        vertex_t u = vertex_queue.top().second;
        vertex_queue.pop();

        // Because we leave old copies of the vertex in the priority queue
        // (with outdated higher distances), we need to ignore it when we come
        // across it again, by checking its distance against the minimum distance
        if (dist > min_distance[u])
            continue;

        // Visit each edge exiting u
        const std::vector<neighbor> &neighbors = adjacency_list[u];
        for (std::vector<neighbor>::const_iterator neighbor_iter = neighbors.begin();
             neighbor_iter != neighbors.end();
             neighbor_iter++)
        {
            vertex_t v = neighbor_iter->target;
            weight_t weight = neighbor_iter->weight;
            weight_t distance_through_u = dist + weight;
            if (distance_through_u < min_distance[v]) {
                min_distance[v] = distance_through_u;
                previous[v] = u;
                vertex_queue.push(std::make_pair(min_distance[v], v));

            }

        }
    }
}


std::list<vertex_t> DijkstraGetShortestPathTo(
        vertex_t vertex, const std::vector<vertex_t> &previous)
{
    std::list<vertex_t> path;
    for ( ; vertex != -1; vertex = previous[vertex])
        path.push_front(vertex);
    return path;
}


struct airport {
    int airportID;
    string name, city, country, IATA, ICAO;
    double latitude, longitude;

    friend bool operator<(airport a, airport b) {
        if(a.airportID < b.airportID)
            return true;
        else
            return false;
    }
};

struct route {
    string sourceAirport;
    int sourceAirportID;
    string destinationAirport;
    int destinationAirportID;
};

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

};

/**
bool Save(const std::string fname, const std::map<edge,int> &c)
{
    std::ofstream f1(fname.c_str(), std::ios::binary);

    if (f1.fail()) return false;

    boost::archive::binary_oarchive oa(f1);
    oa << c;
    f1.close();


    return true;
}**/


int maxAirportID = 0;
vector<airport> ParisIDS;

vector<route> getRoutes() {
    vector<route> routesList;
    vector<string> tmpSegmentLine;
    ifstream infile("routes.dat");
    stringstream ss;
    string tmp;
    route tmpRoute;

    for( string line; getline( infile, line ); ) {
        istringstream ss(line);
        string token;

        while (getline(ss, token, ',')) {
            token.erase(remove(token.begin(), token.end(), '"'), token.end());
            tmpSegmentLine.push_back(token);
        }

        if(tmpSegmentLine.at(5) != "\\N" && tmpSegmentLine.at(3) != "\\N") {
            tmpRoute = {
                    tmpSegmentLine.at(2),
                    stoi(tmpSegmentLine.at(3).c_str()),
                    tmpSegmentLine.at(4),
                    stoi(tmpSegmentLine.at(5).c_str())
            };


            routesList.push_back(tmpRoute);
        }
        tmpSegmentLine.clear();

    }


    return routesList;
}

map<int,airport> getAirports(){
    map<int,airport> airportList;
    vector<string> tmpSegmentLine;
    ifstream infile("airports.dat");
    stringstream ss;
    string tmp;
    airport tmpAirport;

    for( string line; getline( infile, line ); )
    {
        istringstream ss(line);
        string token;

        while(getline(ss, token, ',')) {
            token.erase(remove(token.begin(), token.end(), '"'), token.end());
            tmpSegmentLine.push_back(token);
        }

        if(stoi(tmpSegmentLine.at(0)) > maxAirportID)
            maxAirportID = stoi(tmpSegmentLine.at(0));

        std::cout << line << std::endl;

        std::cout << "x:"<< tmpSegmentLine.at(6) << ",y:" << tmpSegmentLine.at(7) << std::endl;

        tmpAirport = {stoi(tmpSegmentLine.at(0)),
                      tmpSegmentLine.at(1),
                      tmpSegmentLine.at(2),
                      tmpSegmentLine.at(3),
                      tmpSegmentLine.at(4),
                      tmpSegmentLine.at(5),
                      stod(tmpSegmentLine.at(6)),
                      stod(tmpSegmentLine.at(7))
        };

        if(tmpSegmentLine.at(2)=="Paris")
            ParisIDS.push_back(tmpAirport);

        airportList.insert(std::pair<int, airport>(stoi(tmpSegmentLine.at(0)),tmpAirport));
        tmpSegmentLine.clear();

    }
    return airportList;
}


template <typename T1, typename T2>
struct less_second {
    typedef pair<T1, T2> type;
    bool operator ()(type const& a, type const& b) const {
        return a.second > b.second;
    }
};


void helper_quadratic_to (cairo_t *cr,
                     double x1, double y1,
                     double x2, double y2)
{
    double x0, y0;
    cairo_get_current_point (cr, &x0, &y0);
    cairo_curve_to (cr,
                    2.0 / 3.0 * x1 + 1.0 / 3.0 * x0,
                    2.0 / 3.0 * y1 + 1.0 / 3.0 * y0,
                    2.0 / 3.0 * x1 + 1.0 / 3.0 * x2,
                    2.0 / 3.0 * y1 + 1.0 / 3.0 * y2,
                    y1, y2);
}

int main() {

    projPJ pj_wsg84 = pj_init_plus("+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs ");
    projPJ pj_merc = pj_init_plus("+proj=merc +lon_0=0 +k=1 +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs ");

    map<edge,int> db;
    map<int,airport> airportsData = getAirports();
    vector<route> routesData = getRoutes();

    int i, j, k;

    edge tmpEdge;

    airport tmpAirport;
    airport ParisCity = {(int) 0, "Paris", "Paris", "France", "Paris", "Paris", 48.8566,2.3522};



    adjacency_list_t adjacency_list(maxAirportID);



    for(i = 0; i < routesData.size(); i++){

        adjacency_list[routesData.at(i).sourceAirportID]
                .push_back(neighbor(routesData.at(i).destinationAirportID,1));
        adjacency_list[routesData.at(i).destinationAirportID]
                .push_back(neighbor(routesData.at(i).sourceAirportID,1));
    }





    std::vector<weight_t> min_distance;
    std::vector<vertex_t> previous;
    std::list<vertex_t> path;


   for(i = 0; i < ParisIDS.size(); i++){
        min_distance.clear();
        previous.clear();
        path.clear();

        DijkstraComputePaths(ParisIDS.at(i).airportID, adjacency_list, min_distance, previous);

        for(std::map<int,airport>::const_iterator it = airportsData.begin();
            it != airportsData.end(); ++it){


            if(!isinf(min_distance[it->first]))
            {
                std::cout << "Distance from " << ParisIDS.at(i).airportID << " to " << it->first;

                std::cout << ": " << min_distance[it->first] << std::endl;
                std::cout << ParisIDS.at(i).city << " to " << airportsData[it->first].city << std::endl;
                path = DijkstraGetShortestPathTo(it->first, previous);
                std::cout << "Path : ";
                std::copy(path.begin(), path.end(), std::ostream_iterator<vertex_t>(std::cout, " "));
                std::cout << std::endl;


                tmpAirport = ParisCity;


                std::list<int>::const_iterator itpath = path.begin();
                itpath++;


                for(;itpath != path.end(); ++itpath){
                    tmpEdge = { tmpAirport.latitude, tmpAirport.longitude,
                                airportsData[*itpath].latitude, airportsData[*itpath].longitude };

                    std::cout << tmpEdge.x0 << "," << tmpEdge.y0 << "--->" << tmpEdge.x1 << "," << tmpEdge.y1 << std::endl;

                    std::map<edge,int>::iterator search = db.find(tmpEdge);

                    if(search != db.end()){
                        search->second++;
                    }
                    else {
                        db.insert(std::pair<edge, int>(tmpEdge, 1));
                    }

                    tmpAirport = airportsData[*itpath];
                }
            }
        }
    }


    /*for(std::map<edge,int>::const_iterator it = db.begin();
        it != db.end(); ++it)
    {

         std::cout << it->first.x0 << ","  << it->first.y0 << "-";
         std::cout << it->first.x1 << "," << it->first.y1 << " ::: " << it->second << std::endl;


    }*/

    vector<pair<edge, int>> mapcopy(db.begin(), db.end());
    sort(mapcopy.begin(), mapcopy.end(), less_second<edge, int>());
    std::vector<pair<edge,int>>::iterator it = mapcopy.begin();

  /*  for(; it != mapcopy.end(); it++){
        //std::cout << it->first.x0 << ","  << it->first.y0 << "-";
       // std::cout << it->first.x1 << "," << it->first.y1 << " ::: " << it->second << std::endl;
    }
*/
    std::cout << "ici 1" << std::endl;

    double minLatWSG84, maxLatWSG84,minLonWSG84, maxLonWSG84;
    double minLatMerc, maxLatMerc, minLonMerc, maxLonMerc;
    double tmpy1,tmpy0, tmpx1, tmpx0;

    cairo_surface_t *surface;
    cairo_t *cr;

    double scale =0.02;

    std::cout << "ici 2" << std::endl;


    minLatWSG84 = ((it->first.x0 < it->first.x1) ? it->first.x0 : it->first.x1);
    maxLatWSG84 = ((it->first.x0 > it->first.x1) ? it->first.x0 : it->first.x1);
    minLonWSG84 = ((it->first.y0 < it->first.y1) ? it->first.y0 : it->first.y1);
    maxLonWSG84 = ((it->first.y0 > it->first.y1) ? it->first.y0 : it->first.y1);

    ++it;

    std::cout << "WSG84 :" << std::endl;
    std::cout << "minLat: " << minLatWSG84 << std::endl;
    std::cout << "minLon: " << minLonWSG84 << std::endl;
    std::cout << "maxLat: " << maxLatWSG84 << std::endl;
    std::cout << "maxLon: " << maxLonWSG84 << std::endl;

    std::cout << "ici 3" << std::endl;

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

    it = mapcopy.begin();



    /*tmpx0 = it->first.x0*DEG_TO_RAD;
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



    */



    int nbEdges = mapcopy.size(),
            count = 0,
            maxLineWidth = 20,
            maxColorGrey = 140,
            lineWidth = 50,
            minX = minLatWSG84,
            maxX = maxLatWSG84,
            minY = minLonWSG84,
            maxY = maxLonWSG84,
            offsetLineWidth = 3,
            cutOff = 6000000;

    int h = (abs(maxX) + abs(minX))/scale,
            w = (abs(maxY) + abs(minY))/scale;



    std::cout << "w:" << w << std::endl;
    std::cout << "h:" << h << std::endl;








    double intensity,colorUsed;

    surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24,w,h);
    cr = cairo_create (surface);

    int x0,y0,x1,y1,center_y,tmpY;
    /*int dx,dy;
    double len,angle,midX,midY,sx,sy, slope = 0.000000015;*/

    cairo_new_path (cr);
    cairo_rectangle(cr, 0, 0, w, h);
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_fill(cr);

    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

    center_y = h/2;

    for( it = mapcopy.begin(); it != mapcopy.end(); ++it){



        intensity = getIntensity(count,nbEdges);


        y0 = (it->first.x0 - minX)/scale;
        x0 = (it->first.y0 - minY)/scale;
        y1 = (it->first.x1 - minX)/scale;
        x1 = (it->first.y1 - minY)/scale;

        /* Mirroring */
        tmpY = y0 - center_y;
        y0 = y0 - 2*tmpY;
        tmpY = y1 - center_y;
        y1 = y1 - 2*tmpY;




        //colorUsed = 1.0-intensity-0.8;
        colorUsed = 0;

        lineWidth = maxLineWidth*intensity +  offsetLineWidth ;


        cairo_set_source_rgb(cr, colorUsed,colorUsed,colorUsed);
        cairo_set_line_width (cr, lineWidth);
        cairo_move_to(cr, x0,y0);
        cairo_line_to(cr, x1,y1);

        //helper_quadratic_to(cr,sx,sy,x1,y1);
        cairo_stroke(cr);

        count++;
        if(count % 100 == 0)
            cout << count << "/" << nbEdges << " paths drawn - " << \
                "grey: "<< colorUsed << " | lineWidth: " << lineWidth << endl;


    }

    cairo_surface_write_to_png(surface, "image.png");



    return 0;
}
