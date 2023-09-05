#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "Graph.cpp"
#include "KlotskiBoard.cpp"

Graph<KlotskiBoard> graph;

int main(int argc, char** argv){
    srand(time(NULL));

    if(argc!= 2) {std::cout << "pass in a board"<<std::endl;return 1;}

    std::cout << "Reading board..." << std::endl;
    KlotskiBoard* b = new KlotskiBoard(argv[1]);
    graph.add_to_stack(b);

    graph.expand_graph_dfs();

    std::cout << "nodes count = " << graph.size() << std::endl;
    graph.mark_distances();

    std::cout << graph.size() << std::endl;

    graph.iterate_physics(100);
    graph.render_json("viewer/klotski_data.json");
    
    return 0;
}
