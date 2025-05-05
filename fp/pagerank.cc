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

    // Checks for the correct number of arguments. Otherwise it prints usage.
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " graph.txt" << " Optional_#Iterations"<< endl;
        return 1; 
    }  

    if (argc == 3) {
        int iterations = atoi(argv[2]);
        if (iterations > 0) {
            ITERATIONS = iterations;
        } else {
            cerr << "Invalid number of iterations. Using default: " << ITERATIONS << endl;
        }
    }


    
    // Read the graph file name from command line arguments
    string filename = argv[1];
    unordered_map<int, vector<int>> out_links;
    unordered_map<int, unordered_set<int>> in_links;
    unordered_set<int> nodes; // To store unique nodes

    auto start = chrono::high_resolution_clock::now();

    // Parse graph file, add edges to out_links and in_links and collect unique nodes
    ifstream infile(filename);
    int src, dst;
    while (infile >> src >> dst) {
        out_links[src].push_back(dst);
        in_links[dst].insert(src);
        nodes.insert(src);
        nodes.insert(dst);
    }

    // Initialize Rank vector values
    int n = nodes.size();
    unordered_map<int, double> rank;
    for (int node : nodes) {
        rank[node] = (1.0 - DAMPING) / n;
    }

    // PageRank main loop. Iterate for a fixed number of times
    for (int t = 0; t < ITERATIONS; t++) {
        unordered_map<int, double> new_rank;

        // Compute new ranks for each node
        for (int node : nodes) {
            double sum = 0.0;
            // Accumulate contributions from all incoming links.
            for (int in_node : in_links[node]) {
                int out_degree = out_links[in_node].size();
                if (out_degree > 0) {
                    sum += rank[in_node] / out_degree; 
                }
            }
            // Apply the PageRank formula to compute this node's new rank for the current iteration
            new_rank[node] = DAMPING * sum + (1.0 - DAMPING) / n;
        }
        rank = move(new_rank);
    }



    // Output top 10 nodes by rank
    vector<pair<int, double>> ranked(rank.begin(), rank.end());
    sort(ranked.begin(), ranked.end(), [](auto& a, auto& b) {
        return a.second > b.second;
    });

    for (int i = 0; i < 10 && i < ranked.size(); i++) {
        cout << ranked[i].first << ":\t" << ranked[i].second << endl;
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << "Total program time: " << duration.count() << " seconds" << endl;

    return 0;
}
