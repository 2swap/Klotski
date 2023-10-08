const int C4_HEIGHT = 6;
const int C4_WIDTH = 7;

#include "json.hpp"
using json = nlohmann::json;
#include "JsonC4Cache.cpp"
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "SteadyState.cpp"
#include "Graph.cpp"
#include "C4Board.cpp"
#include "AlphaBetaConnect4.cpp"

void alpha_beta(){
    // Initialize your Connect 4 board
    C4Board initial_board("2202222117077770704435");
    print_ab_blurb(initial_board);
}

void steady_it(){
    SteadyState ss;
    find_steady_state("220222211707777070", 100000000, ss, true, false);
    exit(0);
}

void graph_it(){
    std::string nodestr = "444443676";
    movecache.ReadCache();

    graph.add_to_stack(new C4Board(nodestr));
    //graph.add_node(new C4Board("43637563356652421"), 0);
    graph.expand_graph_dfs();

    graph.make_edges_bidirectional();
    graph.sanitize_for_closure();
    graph.mark_distances();

    std::cout << "nodes count = " << graph.size() << std::endl;
    graph.iterate_physics(1000, true);
    graph.render_json("viewer/data/" + nodestr + ".json");
    movecache.WriteCache();
}

int main(int argc, char** argv){
    //std::cout << "TODO write and read from position cache using hashes along with board strings" << std::endl;
    //exit(1);
    c4_unit_tests();
    steady_state_unit_tests();
    alpha_beta_unit_tests();
    srand(time(NULL));

    graph_it();
    
    return 0;
}
