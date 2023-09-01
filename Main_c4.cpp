const int C4_HEIGHT = 6;
const int C4_WIDTH = 7;

#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "json.hpp"
#include "SteadyState.cpp"
#include "Graph.cpp"
#include "C4Board.cpp"
using json = nlohmann::json;

int main(int argc, char** argv){
    SteadyState ss;
    find_steady_state("4444443552522", 100000000, ss, true, false);
    exit(0);

    c4_unit_tests();
    steady_state_unit_tests();
    srand(time(NULL));

    graph.add_node(new C4Board("4444445623366"), 0);
    //graph.add_node(new C4Board("43637563356652421"), 0);
    graph.expand_graph();
    graph.sanitize_for_closure();

    std::cout << "nodes count = " << graph.size() << std::endl;
    graph.iterate_physics(1000);
    graph.render_json("viewer_c4/data.json");
    
    return 0;
}
