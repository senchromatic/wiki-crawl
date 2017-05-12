#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "analysis.h"
#include "constants.h"
#include "graph.h"

using std::cout;
using std::ostringstream;
using std::string;

int main() {
    cout << '\n';

    vector<Graph> graphs;
    load_graphs(graphs);

    process(graphs);

    return 0;
}

vector<string> parse_csv(const string &str) {
    vector<string> parsed;
    string cell;
    for (const char c : str) {
        if (c == ',') {
            parsed.push_back(cell);
            cell = "";
        }
        else
            cell += c;
    }
    parsed.push_back(cell);
    return parsed;
}

void load_graphs(vector<Graph> &graphs) {
    cout << "Loading...\n\n";

    for (const string &discipline : DISCIPLINES) {
        Graph graph(discipline);
        const string filepath = FOLDER_OUTPUT + discipline + FILE_SUFFIX_OUTPUT;

        ifstream input(filepath);
        string header_row;
        getline(input, header_row);
        string line;
        while (getline(input, line)) {
            const vector<string> parsed_line = parse_csv(line);
            const Vertex from = parsed_line[0], to = parsed_line[1];
            const double tf_idf = atof(parsed_line[2].c_str());
            const Edge edge = make_pair(from, to);
            graph.add_edge(edge);
        }
        input.close();

        graphs.push_back(graph);
    }

    cout << '\n';
}

string format_component_as_set(const Component &component) {
    ostringstream oss;
    oss << "{";
    bool first = true;
    for (const Vertex &v : component) {
        if (!first) oss << ", ";
        first = false;
        oss << v;
    }
    oss << '}';
    return oss.str();
}

void process(vector<Graph> &graphs) {
    cout << "Processing...\n\n";

    for (Graph &graph : graphs) {

        cout << graph.basic_summary();

        graph.floyd_warshall();

        graph.compute_eccentricities();

        cout << "Radius: " << dist2str(graph.compute_radius()) << '\n';
        cout << "Diameter: " << dist2str(graph.compute_diameter()) << '\n';

        cout << "Center (central vertices): " << format_component_as_set(graph.find_center()) << '\n';
        cout << "Periphery (periperhal vertices): " << format_component_as_set(graph.find_periphery()) << '\n';

        cout << "Connectedness: " << graph.is_connected() << '\n';
        cout << "Average clustering coefficient: " << graph.compute_average_clustering_coefficient() << '\n';

        const auto &sccs = graph.find_strongly_connected_components();
        int components = sccs.size(), c = 0;
        cout << "Number of strongly connected components: " << components << '\n';
        for (const auto &component : sccs)
            cout << "Component #" << ++c << " = " << format_component_as_set(component) << '\n';

        graph.compute_pageranks();
        cout << "<original graph>\n" << graph.pagerank_summary() << "</original graph>\n";

        Graph transpose = graph.transpose();
        transpose.process();
        cout << "<transpose graph>\n" << transpose.pagerank_summary() << "</transpose graph>\n";

        cout << '\n';
    }

    cout << '\n';
}
