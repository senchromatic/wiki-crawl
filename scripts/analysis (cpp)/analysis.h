#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "graph.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

vector<string> parse_csv(const string &str);
void load_graphs(vector<Graph> &graphs);
void process(vector<Graph> &graphs);
string format_component_as_set(const Component &component);

#endif // ANALYSIS_H
