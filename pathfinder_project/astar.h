#ifndef ASTAR_INCLUDED
#define ASTAR_INCLUDED

#include <memory>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include "base_map.h"

class astar {
private:
	//NODE NESTED CLASS
	class node {
	private:
		//position in the map and the node's unique id# - calculated by its position in the map
		int i_pos, j_pos, node_id;
		//distance to end node from node's position
		double dist_to_end;
		//pointer to the previous node
		node* prev_node{ nullptr };
		//pointer to the parent map
		base_map* parent_map;
	public:
		//CONSTRUCTORS/DESTRUCTORS
		node(const int, const int, base_map&, node*);
		~node();
		//GETTERS
		int get_nodeid()const;
		int get_ipos()const;
		int get_jpos()const;
		node* get_prev_node()const;
		int get_steps()const;
		double get_dist_to_end()const;
		//SETTERS
		void setPrevNode(node*);
		//EXPAND NODE - for finding surrounding nodes
		std::vector<std::unique_ptr<node>> expand_node()const;
	};

	//pointer to the parent map being searched
	base_map* map_to_search;
	//ID# of start and end nodes
	int startID, endID;
	//vectors for queues and the nodes found during expansion
	std::vector<std::unique_ptr<node>> open_queue;
	std::vector<std::unique_ptr<node>> closed_queue;
	std::vector<std::unique_ptr<node>> found_nodes;

	//PRIVATE FUNCTIONS - only callable by find_path
	void expand_top();
	//insert found nodes into the open queue
	void found_to_open();
	//find the path to a current node
	void get_path_to_node(node&, std::vector<std::vector<int>>&)const;
public:
	//CONSTRUCTORS/DESTRUCTORS
	astar(base_map&);
	~astar();
	//MAIN ALGORITHM CALL
	std::vector<std::vector<int>> find_path();
};

#endif