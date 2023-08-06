#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "Graph.cpp"
#include "json.hpp"
using json = nlohmann::json;

Graph<Board*> graph;

int main(int argc, char** argv){
	srand(time(NULL));

	std::cout << "Reading board..." << std::endl;


    std::ifstream file("Connect 4.json");
    std::string line;

    std::getline(file, line);

    // Read the JSON file
    json j = json::parse(file);

    for (auto& element : j.items()) {
        std::string key = element.key();
        // Add all substrings of the key to the graph
        for (size_t i = 0; i <= key.length(); ++i) {
            std::string sub_key = key.substr(0, i);
            Board* board = new Board(sub_key);
            graph.add_node(board, 0);
        }
    }
    file.close();

	graph.sanitize_for_closure();
	graph.expand_graph();
	std::cout << "nodes count = " << graph.size() << std::endl;
	graph.iterate_physics_and_render(10000);
    
	return 0;
}
