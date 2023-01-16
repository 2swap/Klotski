#ifndef GRAPH_H
#define GRAPH_H

#include <iomanip>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <queue>
#include <random>
#include <limits.h>
#include "Board.cpp"

double dist_bound = -1;

template <class T>
class Node {
public:
	Node(T t, int dist) : data(t), dist(dist) {
		std::unordered_set<T> neighbor_nodes = data->get_neighbors();
		for(auto it = neighbor_nodes.begin(); it != neighbor_nodes.end(); ++it){
			neighbors.insert((*it)->get_hash());
			delete *it;
		}
	}
	int dist = 0;
	int solution_dist = -1;
	T data;
	std::unordered_set<double> neighbors;
	bool flooded = false;
	double x = (double) rand() / (RAND_MAX), y = (double) rand() / (RAND_MAX), z = (double) rand() / (RAND_MAX);
	double vx = 0, vy = 0, vz = 0;
	bool physics_new = dist_bound<0;
};

template <class T>
class Graph{
public:

	Graph(){
		nodes = std::unordered_map<double, Node<T>>();
	}

	~Graph() {
		while(nodes.size()>0){
			auto i = nodes.begin();
			delete i->second.data;
			nodes.erase(i->first);
		}
	}

	void add_node(T t, int dist){
		Node<T> n(t, dist);
		double hash = t->get_hash();
		nodes.insert(std::make_pair(hash,n));
		if(nodes.size()%100 == 0) std::cout << nodes.size() << " nodes and counting..." << std::endl;
		bfs_queue.push(std::make_pair(hash,dist+1));
		dist_count.insert(std::make_pair(dist, std::make_pair(0,0)));
		if(t->is_solution())
			dist_count[dist].second++;
		else
			dist_count[dist].first++;
	}

	/*void node_distance(double id1, double id2){
		Node<T>* node1 = &(nodes.find(id1)->second);
		Node<T>* node2 = &(nodes.find(id2)->second);

		bfs_queue.clear();
		bfs_queue.push(id1, *node1)
		while(!bfs_queue.empty()){
			auto pop = bfs_queue.front();
			bfs_queue.pop();
			double id = pop.first;
			int dist = pop.second;
			std::unordered_set<T> neighbor_nodes = nodes.find(id)->second.data->get_neighbors();
			for(auto it = neighbor_nodes.begin(); it != neighbor_nodes.end(); ++it){
				if(nodes.find((*it)->get_hash()) == nodes.end())
					add_node(*it, dist);
			}
		}
	}
	TODO - doesnt work yet*/

	void expand_graph(){
		while(!bfs_queue.empty()){
			auto pop = bfs_queue.front();
			bfs_queue.pop();
			double id = pop.first;
			int dist = pop.second;
			std::unordered_set<T> neighbor_nodes = nodes.find(id)->second.data->get_neighbors();
			for(auto it = neighbor_nodes.begin(); it != neighbor_nodes.end(); ++it){
				if(nodes.find((*it)->get_hash()) == nodes.end())
					add_node(*it, dist);
			}
		}
	}

	std::set<double> get_solutions(){
		std::set<double> solutions;
		for(auto it = nodes.begin(); it != nodes.end(); ++it){
			double id = it->first;
			Node<T>* node = &(it->second);
			if(node->data->is_solution())
				solutions.insert(id);
		}
		return solutions;
	}

	void mark_solution_dists(std::set<double> solutions){
		bfs_queue = std::queue<std::pair<double, int>>(); // clear it
		for(double solution : solutions){
			bfs_queue.push(std::make_pair(solution, 0));
			nodes.find(solution)->second.solution_dist = 0;
		}
		std::cout << solutions.size() << std::endl;
		while(!bfs_queue.empty()){
			auto pop = bfs_queue.front();
			bfs_queue.pop();
			double id = pop.first;
			int dist = pop.second;
			for(auto it = nodes.find(id)->second.neighbors.begin(); it != nodes.find(id)->second.neighbors.end(); ++it){
				double neighborhash = *it;
				Node<T>* neighbor = &(nodes.find(neighborhash)->second);
				if(neighbor->solution_dist == -1){
					neighbor->solution_dist = dist+1;
					bfs_queue.push(std::make_pair(neighborhash,dist+1));
				}
			}
		}
	}

	void remove_leaves(bool repeat){
		int presize = nodes.size();
		std::queue<double> leaves;
		for(auto it = nodes.begin(); it != nodes.end(); ++it){
			double id = it->first;
			Node<T>* node = &(it->second);
			if(node->neighbors.size() == 1)
				leaves.push(id);
		}
		while(!leaves.empty()){
			remove_node(leaves.front());
			leaves.pop();
		}
		if(presize != nodes.size() && repeat)
			remove_leaves(true);
	}

	void squish_hubs(){
		std::queue<double> hubs;
		for(auto it = nodes.begin(); it != nodes.end(); ++it){
			double id = it->first;
			Node<T>* node = &(it->second);
			if(node->neighbors.size() > 6)
				hubs.push(id);
		}
		while(!hubs.empty()){
			squish_node(hubs.front());
			hubs.pop();
		}
	}

	void squish_components(){
		std::queue<double> components;
		for(auto it = nodes.begin(); it != nodes.end(); ++it){
			double id = it->first;
			Node<T>* node = &(it->second);
			auto neighbors = node->neighbors;
			bool should_include = true;
			for (double d : neighbors){
				if(nodes.find(d)->second.neighbors.size() <= 2){
					should_include = false;
					break;
				}
			}
			if(neighbors.size() > 2 && should_include)
				components.push(id);
		}
		while(!components.empty()){
			squish_node(components.front());
			components.pop();
		}
	}

	bool node_doesnt_exist(double id){
		return nodes.find(id) == nodes.end();
	}

	void remove_node(double id){
		if(node_doesnt_exist(id)) return;
		Node<T>* node = &(nodes.find(id)->second);
		std::unordered_set<double> neighbor_nodes = node->neighbors;
		for(double neighbor_id : neighbor_nodes){
			Node<T>* neighbor = &(nodes.find(neighbor_id)->second);
			neighbor->neighbors.erase(id);
		}
		delete node->data;
		nodes.erase(id);
	}

	void remove_nodes(Graph<T> subgraph){
		for(auto it = subgraph.nodes.begin(); it != subgraph.nodes.end(); ++it){
			remove_node(it->first);
		}
	}

	void squish_node(double id){
		if(node_doesnt_exist(id)) return;
		Node<T>* node = &(nodes.find(id)->second);
		std::unordered_set<double> neighbor_nodes = node->neighbors;
		for(double neighbor_id : neighbor_nodes){
			Node<T>* neighbor = &(nodes.find(neighbor_id)->second);
			std::unordered_set<double> second_neighbors = neighbor->neighbors;
			for(double second_neighbor_id : second_neighbors){
				if(second_neighbor_id == id) continue;
				Node<T>* second_neighbor = &(nodes.find(second_neighbor_id)->second);
				second_neighbor->neighbors.erase(neighbor_id);
				second_neighbor->neighbors.insert(id);
				node->neighbors.insert(second_neighbor_id);
			}
			node->neighbors.erase(neighbor_id);
			delete neighbor->data;
			nodes.erase(neighbor_id);
		}
	}

	double approach_origin(double id){
		if(node_doesnt_exist(id)) return 0;
		Node<T>* node = &(nodes.find(id)->second);
		std::unordered_set<double> neighbor_nodes = node->neighbors;
		for(double neighbor_id : neighbor_nodes){
			Node<T>* neighbor = &(nodes.find(neighbor_id)->second);
			if(neighbor->dist == node->dist-1)
				return neighbor_id;
		}
		return 0;
	}

	void iterate_physics_and_render(int iterations){
		for(int i = 0; i < iterations; i++) {
			std::cout << "Spreading out graph, iteration " << i << std::endl;
			physics_engine();
			render_json(root_node_hash);
		}
	}

	void physics_engine(){
		for(auto it = nodes.begin(); it != nodes.end(); ++it){
			Node<T>* node = &(it->second);
			if(node->physics_new) {
				double nid = approach_origin(it->first);
				if(node_doesnt_exist(nid)) continue;
				Node<T>* happyneighbor = &(nodes.find(nid)->second);
				node->x = happyneighbor->x + (double) rand() / (RAND_MAX);
				node->y = happyneighbor->y + (double) rand() / (RAND_MAX);
				node->z = happyneighbor->z + (double) rand() / (RAND_MAX);
				node->vx = happyneighbor->vx;
				node->vy = happyneighbor->vy;
				node->vz = happyneighbor->vz;
			}
		}

		for(auto it = nodes.begin(); it != nodes.end(); ++it){
			Node<T>* node = &(it->second);
			if(node->dist > dist_bound) continue;
			node->physics_new = false;
			if(symmetrical && node->data->is_right()) continue;
			for(auto it2 = nodes.begin(); it2 != nodes.end(); ++it2){
				Node<T>* node2 = &(it2->second);
				if(node2->dist > dist_bound || it == it2) continue;

				double dx = node2->x - node->x;
				double dy = node2->y - node->y;
				double dz = node2->z - node->z;
				double dist = std::sqrt(dx*dx+dy*dy+dz*dz);

				double invdist = (.3/(dist+.1))/dist;
				double nx = invdist * dx;
				double ny = invdist * dy;
				double nz = invdist * dz;

				node2->vx += nx;
				node2->vy += ny;
				node2->vz += nz;
				node->vx -= nx;
				node->vy -= ny;
				node->vz -= nz;
			}
			std::unordered_set<double> neighbor_nodes = node->neighbors;
			for(double neighbor_id : neighbor_nodes){
				Node<T>* neighbor = &(nodes.find(neighbor_id)->second);
				if(neighbor->dist > dist_bound) continue;

				double dx = node->x - neighbor->x;
				double dy = node->y - neighbor->y;
				double dz = node->z - neighbor->z;
				double dist = std::sqrt(dx*dx+dy*dy+dz*dz);
				double force = (.2*(dist-1))/dist;
				if(rushhour || true) force *= force;
				double nx = force * dx;
				double ny = force * dy;
				double nz = force * dz;
				neighbor->vx += nx;
				neighbor->vy += ny;
				neighbor->vz += nz;
				node->vx -= nx;
				node->vy -= ny;
				node->vz -= nz;
			}
		}

		double decay = .9;

		for(auto it = nodes.begin(); it != nodes.end(); ++it){
			Node<T>* node = &(it->second);
			if(node->dist > dist_bound) continue;
			if(symmetrical && node->data->is_right()) continue;
			node->vx *= decay;
			node->vy *= decay;
			node->vz *= decay;
			node->x += node->vx;
			node->y += node->vy;
			node->z += node->vz;
		}

		if(symmetrical){
			for(auto it = nodes.begin(); it != nodes.end(); ++it){
				Node<T>* node = &(it->second);
				if(node->dist > dist_bound) continue;
				if(!node->data->is_right()) continue;
				Node<T>* pair = &(nodes.find(it->second.data->get_reverse_hash())->second);
				node->x = -pair->x;
				node->y = pair->y;
				node->z = pair->z;
			}
		}
	}

	void render_json(double root_node_hash){
		std::ofstream myfile;
		myfile << std::setprecision(std::numeric_limits<double>::digits10 + 2);
		myfile.open ("viewer/data.json");
		myfile << "var blurb = '" + nodes.find(root_node_hash)->second.data->blurb + "';\n";
		myfile << "var nodes_to_use = {\n";
		for(auto it = nodes.begin(); it != nodes.end(); ++it){
			myfile << "\t\"" << it->first << "\":{\"dist\":" << it->second.dist << ",\"solution_dist\":" << it->second.solution_dist << ",\"x\":" << it->second.x << ",\"y\":" << it->second.y << ",\"z\":" << it->second.z << ",\"representation\":\"" << it->second.data->representation << "\",\"neighbors\":[";
			auto neighborit = it->second.neighbors.begin();
			myfile << "\"" << *neighborit << "\"";
			while (neighborit != it->second.neighbors.end()){
				myfile << ",\"" << *neighborit << "\"";
				++neighborit;
			}
			myfile << "]},\n";
		}
		myfile << "}\n";

		myfile << "var histogram_non_solutions = [";
		for(auto it = dist_count.begin(); it != dist_count.end(); ++it)
			myfile << it->second.first << ",";
		myfile << "];" << std::endl;

		myfile << "var histogram_solutions = [";
		for(auto it = dist_count.begin(); it != dist_count.end(); ++it)
			myfile << it->second.second << ",";
		myfile << "];" << std::endl;

		myfile << "var board_string = \"" << nodes.find(root_node_hash)->second.data->representation << "\";" << std::endl;
		myfile << "var rushhour = " << rushhour << ";" << std::endl;
		myfile << "var board_w = \"" << nodes.find(root_node_hash)->second.data->w << "\";" << std::endl;
		myfile << "var board_h = \"" << nodes.find(root_node_hash)->second.data->h << "\";" << std::endl;

		myfile.close();

		dist_bound+=.125;
	}

	int size(){
		return nodes.size();
	}

	std::unordered_map<double, Node<T>> nodes;
	std::queue<std::pair<double, int>> bfs_queue;
	std::map<int, std::pair<int, int>> dist_count;
	double root_node_hash = 0;
};

#endif
