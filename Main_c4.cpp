const int C4_HEIGHT = 6;
const int C4_WIDTH = 7;

#include "json.hpp"
using json = nlohmann::json;
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "SteadyState.cpp"
#include "Graph.cpp"
#include "C4Board.cpp"

int main(int argc, char** argv){
    c4_unit_tests();
    steady_state_unit_tests();
    srand(time(NULL));

    //SteadyState ss;
    //find_steady_state("436766444174331", 100000000, ss, true, false);
    //exit(0);

    graph.add_node(new C4Board("4366"));
    //graph.add_node(new C4Board("43637563356652421"), 0);
    graph.expand_graph(true);
    graph.sanitize_for_closure();
    graph.mark_distances();

    std::cout << "nodes count = " << graph.size() << std::endl;
    graph.iterate_physics(1000);
    graph.render_json("viewer_c4/data.json");
    
    return 0;
}
