#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "Graph.cpp"
#include "Board.cpp"

Graph<Board*> graph;
Graph<Board*> graph1;

int main(int argc, char** argv){
    srand(time(NULL));

    if(argc!= 2) {std::cout << "pass in a board"<<std::endl;return 1;}

    std::cout << "Reading board..." << std::endl;
    Board* b = new Board(argv[1]);
    graph.add_node(b, 0);
    graph.root_node_hash = b->get_hash();

    graph.expand_graph();

    auto solutions = graph1.get_solutions();
    std::cout << "solutions count = " << solutions.size() << std::endl;
    std::cout << "nodes count = " << graph1.size() << std::endl;
    graph.mark_distances(solutions);

    std::cout << graph.size() << std::endl;

    graph.iterate_physics_and_render(100);
    
    return 0;
}
