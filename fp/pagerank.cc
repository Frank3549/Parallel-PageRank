#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <chrono>
#include <omp.h>
using namespace std;

/*
 * Implements PageRank using parallel (OpenMP) and SIMD acceleration.
 * - Reads a directed graph from a file (src dst format)
 * - Maps each node to a index
 * - Builds in-link and out-link adjacency lists
 * - Runs PageRank iterations with OpenMP+SIMD for performance
 * - Outputs the top 10 nodes by rank and runtime
 */

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
        // node_to_index[node_id] = index
        node_to_index[nodes[i]] = i;
    }

    int unique_nodes = nodes.size();
    vector<double> rank(unique_nodes, (1.0 - DAMPING) / unique_nodes); // Initialize ranks
    vector<vector<int>> out_links(unique_nodes);
    vector<vector<int>> in_links(unique_nodes);


    // node_to_index[node_id] = idx; maps each unique node ID to a index
    // out_links[idx] = [indices of nodes that this node points to]
    for (const auto& pair : out_links_map) { 
        int node_id = pair.first;
        vector<int> node_links = pair.second;
        int idx = node_to_index[node_id]; // pair.first is the node_id
        for (int neighbor : node_links) {
            out_links[idx].push_back(node_to_index[neighbor]);
        }
    }

    // node_to_index[node_id] = idx; maps each unique node ID to a index
    // in_links[idx] = [indices of nodes that point to this node]
    for (const auto& pair : in_links_map) {
        int node_id = pair.first;
        const unordered_set<int>& node_links = pair.second;
        int idx = node_to_index[node_id]; 
        for (int neighbor : node_links) {
            in_links[idx].push_back(node_to_index[neighbor]);
        }
    }

    // Start timer for PageRank computation
    auto start = chrono::high_resolution_clock::now();

    // Main PageRank iteration loop
    for (int iter = 0; iter < ITERATIONS; iter++) {
        vector<double> new_rank(unique_nodes);

        #pragma omp parallel for 
        for (int node = 0; node < unique_nodes; node++) {
            double sum = 0.0;
            // For each incoming link, add its contribution to the rank of the current node
            #pragma omp simd reduction(+:sum)
            for (size_t i = 0; i < in_links[node].size(); i++) {
                int in_idx = in_links[node][i];
                sum += rank[in_idx] / out_links[in_idx].size();
            }
            new_rank[node] = DAMPING * sum + (1.0 - DAMPING) / unique_nodes;
        }
        rank = move(new_rank);
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