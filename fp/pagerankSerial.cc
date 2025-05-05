#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <chrono>

using namespace std;

const double DAMPING = 0.80;
int ITERATIONS = 100; // Default number of iterations

int main(int argc, char* argv[]) {
    // Check for correct number of command-line arguments
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " graph.txt Optional_#Iterations" << endl;
        return 1;
    }

    // If a custom iteration count is provided, use it
    if (argc == 3) {
        int iterations = atoi(argv[2]);
        if (iterations > 0) {
            ITERATIONS = iterations;
        } else {
            cerr << "Invalid number of iterations. Using default: " << ITERATIONS << endl;
        }
    }

    
    string filename = argv[1];
    // Maps and sets are used for setup. Once the graph is built, we can convert to vectors for SIMD operations.
    unordered_map<int, vector<int>> out_links_map;
    unordered_map<int, unordered_set<int>> in_links_map;
    unordered_set<int> node_ids;

    auto start = chrono::high_resolution_clock::now();

    // Parse input file and build graph structure
    ifstream infile(filename);
    int src, dst;
    while (infile >> src >> dst) {
        out_links_map[src].push_back(dst);
        in_links_map[dst].insert(src);
        node_ids.insert(src);
        node_ids.insert(dst);
    }

    // Create ordered list of node IDs and mapping to index for O(1) access in the pagerank algorithm
    vector<int> nodes(node_ids.begin(), node_ids.end());
    sort(nodes.begin(), nodes.end());
    unordered_map<int, int> node_to_index;
    for (size_t i = 0; i < nodes.size(); i++) {
        node_to_index[nodes[i]] = i;
    }

    int unique_nodes = nodes.size();
    vector<double> rank(unique_nodes, (1.0 - DAMPING) / unique_nodes); // Initialize ranks
    vector<vector<int>> out_links(unique_nodes);
    vector<vector<int>> in_links(unique_nodes);

    // Convert graph to indexed vectors for faster access (prepares for SIMD/OpenMP)
    // For each original node (pair.first), map its neighbors to their indices 
    for (const auto& pair : out_links_map) {
        int idx = node_to_index[pair.first];
        for (int neighbor : pair.second) {
            out_links[idx].push_back(node_to_index[neighbor]);
        }
    }

    for (const auto& pair : in_links_map) {
        int idx = node_to_index[pair.first];
        for (int neighbor : pair.second) {
            in_links[idx].push_back(node_to_index[neighbor]);
        }
    }

    // Main PageRank iteration loop
    for (int iter = 0; iter < ITERATIONS; iter++) {
        vector<double> new_rank(unique_nodes);
        for (int node = 0; node < unique_nodes; node++) {
            double sum = 0.0;
            // Iterate over incoming links, calculating the contribution to the rank
            for (int in_idx : in_links[node]) {
                int out_degree = out_links[in_idx].size();
                sum += rank[in_idx] / out_degree;
            }
            new_rank[node] = DAMPING * sum + (1.0 - DAMPING) / unique_nodes;
        }
        rank = move(new_rank); // Efficient rank update
    }

    // Collect and sort top-10 ranked nodes
    vector<pair<int, double>> ranked;
    for (int i = 0; i < unique_nodes; i++) {
        ranked.emplace_back(nodes[i], rank[i]);
    }

    sort(ranked.begin(), ranked.end(), [](auto& a, auto& b) {
        return a.second > b.second;
    });

    for (size_t i = 0; i < 10 && i < ranked.size(); i++) {
        cout << ranked[i].first << ":\t" << ranked[i].second << endl;
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Total program time: " << duration.count() << " seconds" << endl;

    return 0;
}
