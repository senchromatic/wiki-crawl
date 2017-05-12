#include "constants.h"
#include "graph.h"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>

using std::ostringstream;
using std::unordered_set;

string dist2str(const int i) {
    if (i == SAFE_INFINITY)
        return "INFINITY";
    return to_string(i);
}

Graph::Graph(const string &new_name) {
    this->my_name = new_name;
    V = 0;
    E = 0;
    density = 0.0;
    radius = SAFE_INFINITY;
    diameter = 0;
}

string Graph::name() const {
    return my_name;
}

string Graph::basic_summary() {
    ostringstream oss;
    oss << name() << '\n';
    oss << "Total number of vertices: " << V << '\n';
    oss << "Total number of edges: " << E << '\n';
    int max_pairs = V*(V-1);
    density = static_cast<double>(E) / max_pairs;
    oss << "Density: " << density << '\n';
    return oss.str();
}

void Graph::add_edge(const Edge &edge) {
    Vertex from = edge.first, to = edge.second;

    // add new vertices to set of vertices
    for (const Vertex &u: {from, to}) {
        if (vertices.count(u)) continue;
        V++;
        vertices.insert(u);
    }

    // add new edge to neighborhood
    for (const Vertex &u : {from, to}) {
        const Vertex &v = (u == from) ? to : from;
        if (!neighborhoods.count(u))
            neighborhoods[u] = Component();
        neighborhoods[u].insert(v);
    }

    // skip parallel edges
    if (adjacency_matrix.count(edge))
        return;
    E++;

    // add new edge to adjacency matrix
    adjacency_matrix.insert(edge);

    // add new edge to adjacency list
    if (!adjacency_list.count(from))
        adjacency_list[from] = vector<Vertex>();
    adjacency_list[from].push_back(to);
}

int Graph::dist(const Edge &edge) {
    if (!distances.count(edge))
        return SAFE_INFINITY;
    return distances[edge];
}

void Graph::floyd_warshall() {

    // initialization
    distances.clear();
    successors.clear();
    for (const Edge &edge : adjacency_matrix) {
        distances[edge] = 1;
        successors[edge] = edge.first;
    }

    // O(|V|^3)
    for (const Vertex &vk : vertices) {
        for (const Vertex &vi : vertices) {
            Edge eik = make_pair(vi, vk);
            for (const Vertex &vj : vertices) {
                Edge ekj = make_pair(vk, vj);
                Edge eij = make_pair(vi, vj);
                int relaxed = dist(eik) + dist(ekj);
                if (relaxed < dist(eij)) {
                    distances[eij] = relaxed;
                    if (successors.count(eik))
                        successors[eij] = successors[eik];
                }
            }
        }
    }
}

Path Graph::shortest_path(const Vertex &u, const Vertex &v) {
    Edge uv = make_pair(u, v);
    if (!connected(uv))
        throw std::invalid_argument("no path exists between " + u + " and " + v);
    Path sp {u};
    Vertex t = u;
    while (t != v) {
        t = successors[make_pair(t, v)];
        sp.push_back(t);
    }
    return sp;
}

void Graph::compute_eccentricities() {
    for (const Vertex &u : vertices) {
        eccentricities[u] = 0;
        for (const Vertex &v : vertices) {
            Edge uv = make_pair(u, v);
            eccentricities[u] = max(eccentricities[u], dist(uv));
        }
    }
}

int Graph::compute_radius() {
    for (const Vertex &u : vertices)
        radius = min(radius, eccentricities[u]);
    return radius;
}

int Graph::compute_diameter() {
    for (const Vertex &u : vertices)
        diameter = max(diameter, eccentricities[u]);
    return diameter;
}

Component Graph::find_center() {
    central_vertices.clear();
    for (const Vertex &v : vertices)
        if (eccentricities[v] == radius)
            central_vertices.insert(v);
    return central_vertices;
}
Component Graph::find_periphery() {
    peripheral_vertices.clear();
    for (const Vertex &v : vertices)
        if (eccentricities[v] == diameter)
            peripheral_vertices.insert(v);
    return peripheral_vertices;
}

bool Graph::connected(Edge &edge) const {
    return successors.count(edge);
}

bool Graph::is_connected() const {
    return diameter < SAFE_INFINITY;
}

double Graph::compute_local_clustering_coefficient(const Vertex &v) {
    Component &neighborhood = neighborhoods[v];
    int n_cliques = 0;
    for (const Vertex &u : neighborhood)
        for (const Vertex &w : neighborhood)
            n_cliques += adjacency_matrix.count(make_pair(u, w));
    int ns = neighborhood.size();
    int n_pairs = ns*(ns-1);
    local_clustering_coefficients[v] = (n_cliques > 0) ? static_cast<double>(n_cliques) / n_pairs : 0.0;
    return local_clustering_coefficients[v];
}

double Graph::compute_average_clustering_coefficient() {
    double total = 0.0;
    for (const Vertex &v : vertices)
        total += compute_local_clustering_coefficient(v);
    return total / V;
}

unordered_set<Component, ComponentHash> Graph::find_strongly_connected_components() {
    strongly_connected_components.clear();
    Component check = vertices;
    while (!check.empty()) {
        Component scc;  // new strongly connected component
        const Vertex &u = *(check.begin());
        scc.insert(u);
        for (const Vertex &w : check) {  // add w to scc?
            bool strongly_connected = true;
            for (const Vertex &v : scc) {  // check each vertex in scc
                Edge forward_edge = make_pair(v, w), back_edge = make_pair(w, v);
                strongly_connected &= connected(forward_edge) && connected(back_edge);
                if (!strongly_connected)
                    break;
            }
            if (strongly_connected)
                scc.insert(w);
        }
        strongly_connected_components.insert(scc);
        for (const Vertex &v : scc)
            check.erase(v);
    }
    return strongly_connected_components;
}

unordered_map<Vertex, PageRank> Graph::compute_pageranks() {
    // initialization
    unordered_map<Vertex, PageRank> prev;
    double nV = V;
    for (const Vertex &v : vertices)
        prev[v] = 1.0 / nV;

    for (int i = 0; i < PAGERANK_ITERATIONS; i++) {
        for (const Vertex &v : vertices)
            pageranks[v] = 0.0;
        for (const Vertex &u : vertices) {
            const auto &children = adjacency_list[u];
            double pagerank_per_child = prev[u] / children.size();
            for (const auto &v : children)
                pageranks[v] += pagerank_per_child;
        }
        // random surfer
        for (const Vertex &v : vertices) {
            double d = PAGERANK_DAMPING;
            double pagerank = pageranks[v];
            pageranks[v] = (1.0 - d) / nV + d * pagerank;
        }
        prev = pageranks;
    }
    return pageranks;
}

string Graph::pagerank_summary() {
    ostringstream oss;
    int npr = pageranks.size();
    assert(TOP_PAGERANKS <= npr);
    assert(BOTTOM_PAGERANKS <= npr);
    vector<Ranking> rankings;
    for (auto iter = pageranks.begin(); iter != pageranks.end(); iter++)
        rankings.push_back(make_pair(iter->first, iter->second));
    sort(rankings.begin(), rankings.end(),
         [] (const Ranking &r1, const Ranking &r2) {
            if (r1.second != r2.second)
                return r1.second > r2.second;
            return r1.first > r2.first;
         });
    oss << "Highest pageranks: \n";
    for (int i = 0; i < TOP_PAGERANKS; i++) {
        const Vertex &page = rankings[i].first;
        const double &ranking = rankings[i].second;
        oss << i+1 << ". " << page << " : " << ranking << '\n';
    }
    oss << "Lowest pageranks: \n";
    for (int i = npr-1; (npr-i) <= BOTTOM_PAGERANKS; i--) {
        const Vertex &page = rankings[i].first;
        const double &ranking = rankings[i].second;
        oss << i+1 << ". " << page << " : " << ranking << '\n';
    }
    return oss.str();
}

Graph Graph::transpose() {
    Graph t("/reverse of/" + name());
    for (const Vertex &u : vertices)
        for (const Vertex &v : adjacency_list[u])
            t.add_edge(make_pair(v, u));
    return t;
}

void Graph::process() {
    floyd_warshall();
    compute_eccentricities();
    compute_radius();
    compute_diameter();
    compute_average_clustering_coefficient();
    find_strongly_connected_components();
    compute_pageranks();
}

