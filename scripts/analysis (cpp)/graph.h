#ifndef GRAPH_H
#define GRAPH_H

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "constants.h"

using namespace std;

using std::hash;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;

typedef string Vertex;
typedef int Distance;
typedef pair<Vertex, Vertex> Edge;  // not (necessarily) connected, could be any pair of points
typedef vector<Vertex> Path;
typedef unordered_set<Vertex> Component;  // not (necessarily) connected
typedef double PageRank;
typedef pair<Vertex, PageRank> Ranking;

struct EdgeHash {
    size_t operator()(const Edge& e) const {
        hash<string> h;
        return h(e.first) ^ h(e.second);
    }
};

struct ComponentHash {
    size_t operator()(const Component& c) const {
        hash<string> h;
        size_t code = BIG_PRIME_32BIT_SALT;
        for (const Vertex &v : c)
            code ^= h(v);
        return code;
    }
};

string dist2str(const int i);

// unweighted, directed, graph without parallel edges
class Graph {
public:
    Graph(const string &new_name);
    string name() const;
    string basic_summary();
    void add_edge(const Edge &edge);
    void floyd_warshall();  // compute all-pairs shortest paths
    void compute_eccentricities();
    int compute_radius();
    int compute_diameter();
    Component find_center();
    Component find_periphery();
    bool is_connected() const;
    double compute_average_clustering_coefficient();
    unordered_set<Component, ComponentHash> find_strongly_connected_components();
    unordered_map<Vertex, PageRank> compute_pageranks();
    string pagerank_summary();
    Graph transpose();
    void process();
private:
    string my_name;
    int V, E;
    double density;
    Component vertices;
    unordered_set<Edge, EdgeHash> adjacency_matrix;  // (sparse matrix)
    unordered_map<Vertex, vector<Vertex>> adjacency_list;
    unordered_map<Vertex, Component> neighborhoods;
    unordered_map<Edge, Distance, EdgeHash> distances;
    int dist(const Edge &edge);
    unordered_map<Edge, Vertex, EdgeHash> successors;
    Path shortest_path(const Vertex &u, const Vertex &v);  // reconstruct on-the-fly
    bool connected(Edge &edge) const;
    unordered_map<Vertex, Distance> eccentricities;
    int radius, diameter;
    Component central_vertices, peripheral_vertices;
    unordered_map<Vertex, double> local_clustering_coefficients;
    double compute_local_clustering_coefficient(const Vertex &v);
    unordered_set<Component, ComponentHash> strongly_connected_components;
    unordered_map<Vertex, PageRank> pageranks;
};

#endif // GRAPH_H
