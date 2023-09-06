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

    graph.expand_graph_bfs();
    graph.make_edges_bidirectional();

    std::cout << "nodes count = " << graph.size() << std::endl;
    graph.mark_distances();

    graph.iterate_physics(1000);
    std::string outname = argv[1];
    size_t lastSlashPos = outname.find_last_of('/');
    if (lastSlashPos != std::string::npos) {
        outname = outname.substr(lastSlashPos + 1);
    }
    graph.render_json("viewer/data/" + outname + ".json");
    
    return 0;
}
