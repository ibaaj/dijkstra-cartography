#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <string>
#include <sstream>
#include <unistd.h>
#include <math.h>
#include <map>
#include <sstream>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>

#include "osmpbfreader.h"
#include "json.hpp"

using json = nlohmann::json;

const std::string baseURL = "http://192.168.99.100:5000/viaroute?alt=false&compression=true&loc=XXXXXX,XXXXXXX&loc=";

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


template <typename T>
std::string to_string(T const& value) {
    std::stringstream sstr;
    sstr << value;
    return sstr.str();
}

size_t write_to_string(void *ptr, size_t size, size_t count, void *stream) {
    ((std::string*)stream)->append((char*)ptr, 0, size*count);
    return size*count;
}

std::vector<std::pair<double,double>> decodePoly(
        std::basic_string<char, std::char_traits<char>, std::allocator<char>> encoded, double precision) {

    size_t index = 0,
            len = encoded.size();
    int lat = 0,
            lng = 0;
    std::vector<std::pair<double,double>> decodedPoints;

    while (index < len) {
        int b, shift = 0, result = 0;
        do {
            b = encoded[index++] - 63;
            result |= (b & 0x1f) << shift;
            shift += 5;
        } while (b >= 0x20);
        int dlat = ((result & 1) != 0 ? ~(result >> 1) : (result >> 1));
        lat += dlat;

        shift = 0;
        result = 0;
        do {
            b = encoded[index++] - 63;
            result |= (b & 0x1f) << shift;
            shift += 5;
        } while (b >= 0x20);
        int dlng = ((result & 1) != 0 ? ~(result >> 1) : (result >> 1));
        lng += dlng;

        double decodeFactor = pow(10, precision);
        decodedPoints.push_back(std::pair<double, double>(lat / decodeFactor, lng / decodeFactor));
    }



    return decodedPoints;
}


bool Save(const std::string fname, const std::map<edge,int> &c, int nodenumber)
{
    std::ofstream f1(fname.c_str(), std::ios::binary),
                    f2("nodenumber.txt",std::ios::out );

    if (f1.fail() || f2.fail()) return false;

    boost::archive::binary_oarchive oa(f1);
    oa << c;
    f1.close();

    f2 << nodenumber << std::endl;
    f2.close();

    return true;
}

struct Routes {

    int nodes;
    std::map<edge,int> db;


    Routes() : nodes(0) {}

    void node_callback(uint64_t, double lon, double lat, const CanalTP::Tags &){
        ++nodes;

       CURL* curl = curl_easy_init();
        std::vector<std::pair<double,double>> tmpPolyline;
        std::pair<double,double> tmpCoord;
        edge tmpEdge;
        int i;

        if (curl) {
            std::string URL = baseURL + to_string(lat)+ "," + to_string(lon);
            curl_easy_setopt(curl, CURLOPT_URL, URL.c_str() );

            std::string response;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            curl_easy_perform(curl);
            curl_easy_cleanup(curl);


            json j = json::parse(response);
            if(!j["route_geometry"].is_null())
            {
                tmpPolyline = decodePoly(j["route_geometry"],6);


                tmpCoord = tmpPolyline[0];

                for(i=1; i < tmpPolyline.size(); ++i){
                    tmpEdge = { tmpCoord.first, tmpCoord.second, tmpPolyline[i].first, tmpPolyline[i].second };
                    std::map<edge,int>::iterator search = db.find(tmpEdge);

                    if(search != db.end()){
                        search->second++;
                    }
                    else {
                        db.insert(std::pair<edge, int>(tmpEdge, 1));
                    }

                    tmpCoord = tmpPolyline[i];
                }
            }



        }




        if(nodes % 1000 == 0){
            Save("edges.map", db,nodes);
            std::cout << "saved : " << nodes << std::endl;

        }





    }


    void way_callback(uint64_t, const CanalTP::Tags &, const std::vector<uint64_t> &){ }
    void relation_callback(uint64_t, const CanalTP::Tags &, const CanalTP::References & ){ }
};

int main(int argc, char** argv) {

    Routes routes;
    CanalTP::read_osm_pbf("eurasia-railway.osm.pbf", routes);

    std::cout << "nc: " << routes.nodes << std::endl;

    Save("edges.map", routes.db,0);
    return 0;
}
