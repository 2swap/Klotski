#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "Graph.cpp"
#include "Board.cpp"

Graph<Board*> graph;
Graph<Board*> graph1;

void generate_all(){
	int sz = 0;
	std::string starter = "....................";
	for(int bx = 0; bx < 3; bx++)
		for(int by = 0; by < 4; by++){
			std::string withsun = starter;
			withsun[by*4+bx] = 'x';
			withsun[(by+1)*4+bx] = 'x';
			withsun[by*4+bx+1] = 'x';
			withsun[(by+1)*4+bx+1] = 'x';
			for(int hx = 0; hx < 3; hx++)
				for(int hy = 0; hy < 5; hy++){
					if(withsun[hy*4+hx] != '.' || withsun[hy*4+hx+1] != '.') continue;
					std::string withhor = withsun;
					withhor[hy*4+hx] = 'h';
					withhor[hy*4+hx+1] = 'h';
					for(int topa = 0; topa < 16; topa++){
						if(withhor[topa] != '.' || withhor[topa+4] != '.') continue;
						std::string a = withhor;
						a[topa] = 'a';
						a[topa+4] = 'a';
						for(int topb = topa+1; topb < 16; topb++){
							if(a[topb] != '.' || a[topb+4] != '.') continue;
							std::string b = a;
							b[topb] = 'b';
							b[topb+4] = 'b';
							for(int topc = topb+1; topc < 16; topc++){
								if(b[topc] != '.' || b[topc+4] != '.') continue;
								std::string c = b;
								c[topc] = 'c';
								c[topc+4] = 'c';
								for(int topd = topc+1; topd < 16; topd++){
									if(c[topd] != '.' || c[topd+4] != '.') continue;
									std::string d = c;
									d[topd] = 'd';
									d[topd+4] = 'd';
									
								for(int tope = 0; tope < 20; tope++){
									if(d[tope] != '.') continue;
									std::string e = d;
									e[tope] = 'e';
								for(int topf = tope+1; topf < 20; topf++){
									if(e[topf] != '.') continue;
									std::string f = e;
									f[topf] = 'f';
								for(int topg = topf+1; topg < 20; topg++){
									if(f[topg] != '.') continue;
									std::string g = f;
									g[topg] = 'g';
								for(int toph = topg+1; toph < 20; toph++){
									if(g[toph] != '.') continue;
									std::string h = g;
									h[toph] = 'h';
									

									char* cstring = new char[h.length()];
									strcpy(cstring, h.c_str()); 
									Board* board = new Board(5, 4, cstring);
									double hash = board->get_hash();
									if(graph.nodes.find(hash) != graph.nodes.end()){delete board;continue;}
									graph.add_node(board, 0);
									graph.expand_graph();
									graph.root_node_hash = hash;
									std::cout << graph.size()-sz << std::endl;
									sz = graph.size();
								}}}}
								}
							}
						}
					}
				}
		}
}

int main(int argc, char** argv){
	srand(time(NULL));
	
	double root_node_hash = 0;
	for(int i = 1; i < argc; i++){
		std::cout << "Reading board " << i << "..." << std::endl;
		Board* b = new Board(argv[i]);
		graph1.add_node(b, 0);
		graph1.root_node_hash = b->get_hash();
	}

	graph1.expand_graph();

	auto solutions = graph1.get_solutions();
	std::cout << "solutions count = " << solutions.size() << std::endl;
	std::cout << "nodes count = " << graph1.size() << std::endl;
	graph1.mark_solution_dists(solutions);

	generate_all();
	graph.remove_nodes(graph1);
	std::cout << graph.size() << std::endl;

	graph1.iterate_physics_and_render(1000);
	
	return 0;
}

//4392*6*5/2 = positions with illegal moves
