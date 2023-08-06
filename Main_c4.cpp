#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "json.hpp"
#include "C4Board.cpp"
#include "Graph.cpp"
#include "SteadyState.cpp"
using json = nlohmann::json;

Graph<C4Board*> graph;

int main(int argc, char** argv){
    steady_state_unit_tests();
    srand(time(NULL));

    std::cout << "Reading board..." << std::endl;

    std::ifstream file("Connect 4.json");
    std::string line;

    std::getline(file, line);

    // Read the JSON file
    json j = json::parse(file);

    /*for (auto& element : j.items()) {
        std::string key = element.key();
        // Add all substrings of the key to the graph
        for (size_t i = 0; i <= key.length(); ++i) {
            std::string sub_key = key.substr(0, i);
            C4Board* board = new C4Board(sub_key);
            graph.add_node(board, 0);
        }
    }*/
    graph.add_node(new C4Board("444444326552252676226333366111773"), 0);
    file.close();

    graph.expand_graph();
    graph.sanitize_for_closure();
    graph.make_edges_bidirectional();
    std::cout << "nodes count = " << graph.size() << std::endl;
    graph.iterate_physics(500);
    graph.render_json("viewer_c4/data.json");
    
    return 0;
}
