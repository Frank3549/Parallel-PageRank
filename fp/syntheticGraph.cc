#include <iostream>
#include <fstream>
#include <random>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

int main() {
    const long long NUM_EDGES = 4000000;  // 4 million edges
    const int NUM_NODES = 10000;         // 10k unique nodes
    const string OUTPUT_FILE = "synthetic_graph.txt";

    // If the output file already exists, we will overwrite it
    if (fs::exists(OUTPUT_FILE)) {
        cout << "File '" << OUTPUT_FILE << "' already exists. Overwriting...\n";
    }

    ofstream out(OUTPUT_FILE, ios::trunc); // overwrite if exists
    if (!out.is_open()) {
        cerr << "Error: Failed to open file for writing.\n";
        return 1;
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, NUM_NODES - 1);

    for (long long i = 0; i < NUM_EDGES; i++) {
        int source = dist(gen);
        int destination = dist(gen);
        out << source << " " << destination << "\n";
        // Print progress every 10 million edges
        if (i % 10000000 == 0) {
            cout << "Generated " << i / 1000000 << " million edges...\n";
        }
    }

    out.close();
    cout << "Done." << endl;

    return 0;
}
