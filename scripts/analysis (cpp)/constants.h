#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <climits>
#include <string>
#include <vector>

using std::string;
using std::vector;

constexpr bool SKIP_HEADER = true;

const string ROOT = "../../";
const string FOLDER_OUTPUT = ROOT + "output/";
const string FILE_SUFFIX_OUTPUT = "_edgelist.csv";
const vector<string> DISCIPLINES = {"economics", "finance", "mathematics", "physics", "politics", "probability", "statistics"};

constexpr int SAFE_INFINITY = INT_MAX/3;
constexpr int BIG_PRIME_32BIT_SALT = 2147483647;  // (2^31) - 1

constexpr double PAGERANK_DAMPING = 0.85;
constexpr int PAGERANK_ITERATIONS = 100;
constexpr int TOP_PAGERANKS = 5;
constexpr int BOTTOM_PAGERANKS = 5;

#endif // CONSTANTS_H

