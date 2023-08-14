#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "json.hpp"
#include "SteadyState.cpp"
#include "C4Board.cpp"
#include "Graph.cpp"
using json = nlohmann::json;

int main(int argc, char** argv){
    c4_unit_tests();
    steady_state_unit_tests();
    srand(time(NULL));

    SteadyState ugh;
    find_steady_state("44444456233333565552166", 100000, ugh);

    std::cout << "Reading board..." << std::endl;

    //std::ifstream file("Connect 4.json");
    //std::string line;

    //std::getline(file, line);

    // Read the JSON file
    //json j = json::parse(file);

    /*for (auto& element : j.items()) {
        std::string key = element.key();
        // Add all substrings of the key to the graph
        for (size_t i = 0; i <= key.length(); ++i) {
            std::string sub_key = key.substr(0, i);
            C4Board* board = new C4Board(sub_key);
            graph.add_node(board, 0);
        }
    }*/
    Graph<C4Board> graph;
    graph.add_node(new C4Board("44444456233333565552166"), 0);
    graph.expand_graph();
    graph.sanitize_for_closure();
    graph.make_edges_bidirectional();

    
    //file.close();

    std::cout << "nodes count = " << graph.size() << std::endl;
    graph.iterate_physics(1500);
    graph.render_json("viewer_c4/data.json");
    
    return 0;
}
