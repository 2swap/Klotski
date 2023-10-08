#include "json.hpp"
using json = nlohmann::json;
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "Graph.cpp"
#include "PegJumpingBoard.cpp"

Graph<PegJumpingBoard> graph;

int main(int argc, char** argv){
    graph.add_to_stack(new PegJumpingBoard(4));
    graph.expand_graph_dfs();

    graph.make_edges_bidirectional();
    graph.sanitize_for_closure();
    graph.mark_distances();

    std::cout << "nodes count = " << graph.size() << std::endl;
    graph.iterate_physics(1000, true);
    graph.render_json("viewer/data/pegs.json");
    
    return 0;
}
