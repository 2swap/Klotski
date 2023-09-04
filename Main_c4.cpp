const int C4_HEIGHT = 6;
const int C4_WIDTH = 7;

//#include <mutex>
//std::mutex mtx;

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
    std::cout << "TODO write and read from position cache using hashes along with board strings" << std::endl;
    exit(1);
    c4_unit_tests();
    steady_state_unit_tests();
    srand(time(NULL));

    //SteadyState ss;
    //find_steady_state("436766444174331", 100000000, ss, true, false);
    //exit(0);

    graph.add_to_stack(new C4Board("4366"));
    //graph.add_node(new C4Board("43637563356652421"), 0);
    graph.expand_graph_dfs();

    /*const int num_threads = 1;

    // Create a vector of threads
    std::vector<std::thread> threads;

    // Start the threads
    for (int i = 0; i < num_threads; ++i) {
        threads.push_back(std::thread(&Graph<C4Board>::expand_graph, &graph));
    }

    // Wait for all threads to finish
    for (std::thread& t : threads) {
        t.join();
    }*/

    graph.sanitize_for_closure();
    graph.mark_distances();

    std::cout << "nodes count = " << graph.size() << std::endl;
    graph.iterate_physics(1000);
    graph.render_json("viewer_c4/data.json");
    
    return 0;
}
