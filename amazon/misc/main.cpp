#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <list>
#include <queue>
#include <map>
#include <cmath>
#include <vector>
#include <set>
#include <ios>
#include <unordered_set>
#include <unordered_map>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/map.hpp>

#include "osmpbfreader.h"

using namespace std;

int indexAmazon,
    osmidAmazon = 2295651;


// C++ Dijkstra source from RosettaCode under GFDL license
// http://rosettacode.org/wiki/Dijkstra%27s_algorithm

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


// parsing
// http://www.h-renrew.de/h/osm/osmchecks/07_watershed/south-america/relations_up.txt
struct route {
    int relationID;
    int source;
};


map<int,int> getRelations(){
    ifstream infile("relations_up.txt");
    stringstream ss;
    string line;
    int pos,indexRelation=0;
    map<int,int> relations;

    for( ; getline( infile, line ); ) {

        pos = line.find_first_of('=');

        relations.insert(std::pair<int,int>(stoi(line.substr(0, pos)),indexRelation));

        if(stoi(line.substr(0, pos))==2295651)
            indexAmazon=indexRelation;

        indexRelation++;
    }
    return relations;
}


vector<route> getRoutes(map<int,int> relations) {
    vector<route> routesList;
    vector<int> sourcesArray;
    ifstream infile("relations_up.txt");
    stringstream ss;
    string line, sources,token;
    route tmpRoute;
    int i,pos;



    infile.clear();
    infile.seekg(0, ios::beg);

    for( ; getline(infile, line);) {
        pos = line.find_first_of('=');

        sources = line.substr(pos+1);

        istringstream ss(sources);
        string token;

        while (getline(ss, token, ' ')) {
            sourcesArray.push_back(stoi(token));
        }

        for(i=0; i < sourcesArray.size(); i++){

            tmpRoute = { relations[stoi(line.substr(0, pos))], relations[sourcesArray.at(i)] };
            routesList.push_back(tmpRoute);
        }
        sourcesArray.clear();

    }


    return routesList;
}


struct nodeDef {

    double lon;
    double lat;
};


struct ParserAmazon {

    int nodes;
    int ways;
    int relations;
    int mode;
    unordered_map<uint64_t,nodeDef> nodeSet;
    unordered_map<uint64_t,vector<uint64_t>> waySet;
    unordered_map<uint64_t,vector<uint64_t>> relationSet;
    set<uint64_t> data;


    ParserAmazon(int md, set<uint64_t> d){
        nodes = 0;
        ways = 0;
        relations = 0;
        data = d;
        mode = md;
    }

    void changeMode(){
        set<uint64_t> tmp;

        if(mode == 2){
            for(unordered_map<uint64_t,vector<uint64_t>>::iterator it = waySet.begin();
                it != waySet.end(); it++){
                tmp.insert(it->second.begin(), it->second.end());

            }
        }
        if(mode == 3){
            for(unordered_map<uint64_t,vector<uint64_t>>::iterator it = relationSet.begin();
                    it != relationSet.end(); it++){
                tmp.insert(it->second.begin(), it->second.end());

            }

        }

        mode--;


    }


    void node_callback(uint64_t osmid, double lon, double lat, const CanalTP::Tags &/*tags*/){
        if(mode == 1){
            if(std::find(data.begin(), data.end(), osmid) != data.end()){
                ++nodes;
                nodeDef n = {  lon,  lat};
                nodeSet.insert(std::pair<uint64_t,nodeDef>(osmid,n));
            }
        }

    }

    void way_callback(uint64_t osmid, const CanalTP::Tags &tags, const std::vector<uint64_t> &refs){
        if(mode == 2){
            if(std::find(data.begin(), data.end(), osmid) != data.end()) {
                ++ways;
                waySet.insert(std::pair<uint64_t, vector<uint64_t>>(osmid, refs));
            }
        }
    }

    void relation_callback(uint64_t osmid, const CanalTP::Tags &tags, const CanalTP::References & refs){

        if(mode == 3){

            if(std::find(data.begin(), data.end(), osmid) != data.end())
            ++relations;
            vector<uint64_t> tmp;
            int i;

            for(i = 0; i < refs.size(); i++){
                tmp.push_back(refs.at(i).member_id);
                std::cout << refs.at(i).member_id << std::endl;
            }

            relationSet.insert(std::pair<uint64_t,vector<uint64_t>>(osmid, tmp));
        }

    }
};


template<typename A, typename B>
std::pair<B,A> flip_pair(const std::pair<A,B> &p)
{
    return std::pair<B,A>(p.second, p.first);
}

template<typename A, typename B>
std::multimap<B,A> flip_map(const std::map<A,B> &src)
{
    std::multimap<B,A> dst;
    std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()),
                   flip_pair<A,B>);
    return dst;
}


struct edgeRelation {
    int relation0,relation1;

    friend bool operator<(edgeRelation a, edgeRelation b) {

        if(a.relation0 < b.relation0 ||
                (a.relation0==b.relation0 && a.relation1 < b.relation1))
            return true;
        else
            return false;
    }
    friend bool operator>(edgeRelation a, edgeRelation b) {
        if(a.relation0 > b.relation0 ||
                (a.relation0==b.relation0 && a.relation1 > b.relation1))
            return true;
        else
            return false;
    }
    bool operator==(edgeRelation a){
        if(relation0==a.relation0 && relation1==a.relation1)
            return true;
        else
            return false;
    }
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
    friend class boost::serialization::access;
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & x0;
        ar & y0;
        ar & x1;
        ar & y1;

    }
};

int main() {
    map<int,int> relations = getRelations();
    vector<route> routes = getRoutes(relations);

    std::vector<weight_t> min_distance;
    std::vector<vertex_t> previous;
    std::list<vertex_t> path;

    adjacency_list_t adjacency_list(relations.size());
    std::cout << "Adj" << std::endl;
    int i;

    route tmpRoute;

    edgeRelation tmpEdgeRelation;

    int tmpRelation;

    map<edgeRelation,int> db;
    std::multimap<int,edgeRelation> sortedDb;

    set<uint64_t> keepRelations;

    std::ofstream writeRelation("relations.txt", std::ios_base::app | std::ios_base::out);


    for(i=0; i < routes.size(); i++){
        tmpRoute = routes.at(i);
        adjacency_list[tmpRoute.relationID]
                .push_back(neighbor(tmpRoute.source,1));
        adjacency_list[tmpRoute.source]
                .push_back(neighbor(tmpRoute.relationID,1));
    }

    DijkstraComputePaths(indexAmazon, adjacency_list, min_distance, previous);


    for(std::map<int,int>::iterator it = relations.begin(); it != relations.end(); it++){


        if(!isinf(min_distance[it->second]))
        {
            std::cout << "Distance from (2295651) " << indexAmazon << " to " << it->second;
            std::cout << ": " << min_distance[it->second] << std::endl;
            path = DijkstraGetShortestPathTo(it->second, previous);
            std::cout << "Path : ";
            std::copy(path.begin(), path.end(), std::ostream_iterator<vertex_t>(std::cout, " "));
            std::cout << std::endl;


            tmpRelation = osmidAmazon;
            path.pop_front();

            for(std::list<int>::const_iterator itpath = path.begin();
                itpath != path.end(); ++itpath) {

                for (map<int,int>::iterator it = relations.begin(); it != relations.end(); ++it ){
                    if (it->second == *itpath){
                        tmpEdgeRelation = { tmpRelation, it->first };
                        std::map<edgeRelation,int>::iterator search = db.find(tmpEdgeRelation);

                        if(search != db.end()){
                            search->second++;
                        }
                        else {

                            db.insert(std::pair<edgeRelation, int>(tmpEdgeRelation, 1));
                        }

                        tmpRelation = it->first;
                    }
                }
        }
    }
    }


    sortedDb = flip_map(db);

    ParserAmazon p(3,keepRelations);

    CanalTP::read_osm_pbf("south-america-latest.osm.pbf", p);

    p.changeMode();

    CanalTP::read_osm_pbf("south-america-latest.osm.pbf", p);

    p.changeMode();

    CanalTP::read_osm_pbf("south-america-latest.osm.pbf", p);

    std::multimap<int,edgeRelation>::iterator itdb = sortedDb.end();
    itdb--; // I don't know why multimap added me a shitty element at the end. skipping.
    for(; itdb != sortedDb.begin(); itdb--){

        unordered_map<uint64_t,vector<uint64_t>>::iterator um = p.relationSet.find(itdb->second.relation0);
        std::cout << "r0:" << itdb->second.relation0 << "-r1:" << itdb->second.relation1 << "=" << itdb->first << std::endl;

    }






    return 0;
}
