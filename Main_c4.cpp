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

    Graph<C4Board> graph;
    graph.add_node(new C4Board("4444445623333"), 0);
    graph.expand_graph();
    graph.sanitize_for_closure();
    graph.make_edges_bidirectional();

    std::cout << "nodes count = " << graph.size() << std::endl;
    graph.iterate_physics(1500);
    graph.render_json("viewer_c4/data.json");
    
    return 0;
}
