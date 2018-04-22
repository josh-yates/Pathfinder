#include "astar.h"

//NODE FUNCTIONS
astar::node::node(const int i_in, const int j_in, base_map& map_in, node* prev_in) {
	//check dimensions are in map
	if (j_in < 0 || j_in >= map_in.get_cols() || i_in < 0 || j_in >= map_in.get_rows()) {
		throw "node constructor: coordinates outside map dimensions";
	}
	//check coordinates are not a wall or obstacle
	if (map_in(i_in, j_in) == wall || map_in(i_in, j_in) == obstacle) {
		throw "node constructor: node cannot be on obstacle/wall";
	}
	//check if it is the start point, if so it must be nullptr
	map_in.find_start_end();
	if ((i_in == map_in.get_start_i() && j_in == map_in.get_start_j()) && prev_in != nullptr) {
		throw "node constructor: start node must have nullptr previous";
	}
	//set variables
	i_pos = i_in;
	j_pos = j_in;
	node_id = (i_pos*map_in.get_cols()) + j_pos;
	prev_node = prev_in;
	parent_map = &map_in;

	//calculate distance to end node
	dist_to_end = pow((pow((map_in.get_end_i() - i_pos), 2) + pow(map_in.get_end_j() - j_pos, 2)), 0.5);
}
astar::node::~node() {
	prev_node = nullptr;
}
int astar::node::get_nodeid()const {
	return node_id;
}
int astar::node::get_ipos()const {
	return i_pos;
}
int astar::node::get_jpos()const {
	return j_pos;
}
astar::node* astar::node::get_prev_node()const {
	return prev_node;
}
int astar::node::get_steps()const {
	if (prev_node == nullptr) {
		return 0;
	}
	else {
		return 1 + prev_node->get_steps();
	}
}
double astar::node::get_dist_to_end()const {
	return dist_to_end;
}
//SETTERS
void astar::node::setPrevNode(node* new_prev_node) {
	prev_node = new_prev_node;
}//EXPAND
std::vector<std::unique_ptr<astar::node>> astar::node::expand_node()const {
	std::vector<std::unique_ptr<node>> surrounding_nodes;
	for (int search_pos{ 1 }; search_pos <= 4; search_pos++) {
		//coordinates to search in the map
		int search_i;
		int search_j;
		switch (search_pos) {
		case 1:
			search_i = i_pos - 1;
			search_j = j_pos;
			break;
		case 2:
			search_i = i_pos;
			search_j = j_pos + 1;
			break;
		case 3:
			search_i = i_pos + 1;
			search_j = j_pos;
			break;
		case 4:
			search_i = i_pos;
			search_j = j_pos - 1;
			break;
		}
		//node id of searched tile
		int search_id{ (search_i*parent_map->get_cols()) + search_j };
		//if it's a free space, start or end and it's not the previous node, add to the vector
		map_point_type found_point{ parent_map->operator()(search_i,search_j) };
		if (found_point == free_space || found_point == start_point || found_point == end_point) {
			if (prev_node == nullptr) {
				surrounding_nodes.push_back(std::move(std::unique_ptr<node>(new node(search_i, search_j, *parent_map, const_cast<node*>(this)))));
			}
			else if (search_id != prev_node->get_nodeid()) {
				surrounding_nodes.push_back(std::move(std::unique_ptr<node>(new node(search_i, search_j, *parent_map, const_cast<node*>(this)))));
			}
		}
	}
	return surrounding_nodes;
}





//ASTAR FUNCTIONS
astar::astar(base_map& map_in) {
	map_to_search = &map_in;
	//calculate start and end ID#
	map_to_search->find_start_end();
	//get the start and end position in the map
	int start_i{ map_to_search->get_start_i() };
	int start_j{ map_to_search->get_start_j() };
	int end_i{ map_to_search->get_end_i() };
	int end_j{ map_to_search->get_end_j() };
	if (start_i < 0 || start_j < 0 || end_i < 0 || end_j < 0) {
		throw "astar constructor: map not valid";
	}
	//calculate the id's of start and end nodes
	startID = (start_i*map_to_search->get_cols()) + start_j;
	endID = (end_i*map_to_search->get_cols()) + end_j;
	//create a unique pointer to the start node and push to open queue
	open_queue.push_back(std::move(std::unique_ptr<node>(new node(start_i, start_j, map_in, nullptr))));
}
astar::~astar() {/*No deletion needed!*/ }

void astar::expand_top() {
	//clear the nullptr's out of found_nodes
	found_nodes.clear();
	//expand the top node
	found_nodes = open_queue.begin()->get()->expand_node();
	//move top node from open to closed
	closed_queue.push_back(std::move(*open_queue.begin()));
	open_queue.erase(open_queue.begin());
}

void astar::found_to_open() {
	//iterate over the found nodes
	for (auto node_it = found_nodes.begin(); node_it != found_nodes.end(); node_it++) {
		//if node ID is in the closed queue, skip it
		auto find_ID_closed = std::find_if(closed_queue.begin(), closed_queue.end(), [&node_it](std::unique_ptr<node>& closed_node) {
			return closed_node->get_nodeid() == node_it->get()->get_nodeid();
		});
		if (find_ID_closed == closed_queue.end()) {
			//node ID not in closed queue. See if it is in open queue, if it is, check the steps
			auto find_ID_open = std::find_if(open_queue.begin(), open_queue.end(), [&node_it](std::unique_ptr<node>& open_node) {
				return open_node->get_nodeid() == node_it->get()->get_nodeid();
			});
			if (find_ID_open != open_queue.end()) {
				//compare steps
				if (node_it->get()->get_steps() < find_ID_open->get()->get_steps()) {
					find_ID_open->get()->setPrevNode(node_it->get()->get_prev_node());
				}
				//else, the one in the queue was shorter
			}
			else {
				//else, it was not in the open queue, so add it
				open_queue.push_back(std::move(*node_it));
			}
		}
	}
	//clear out the found nodes
	found_nodes.clear();
	//sort the open queue
	std::sort(open_queue.begin(), open_queue.end(), [](std::unique_ptr<node>& node_a, std::unique_ptr<node>& node_b) {
		return (node_a->get_dist_to_end() + node_a->get_steps()) < (node_b->get_dist_to_end() + node_b->get_steps());
	});
}

void astar::get_path_to_node(node& curr_node, std::vector<std::vector<int>>& path_coords)const {
	if (curr_node.get_prev_node() != nullptr) {
		get_path_to_node(*curr_node.get_prev_node(), path_coords);
	}
	//push back the coordinates to a vector, then push that onto path coords
	std::vector<int> node_coords;
	node_coords.push_back(curr_node.get_ipos());
	node_coords.push_back(curr_node.get_jpos());
	path_coords.push_back(node_coords);
}

std::vector<std::vector<int>> astar::find_path() {
	//array to store coordinates
	std::vector<std::vector<int>> path_coords;
	bool still_searching{ true };
	while (still_searching) {
		//check there are still nodes to expand
		if (open_queue.size() == 0) {
			still_searching = false;
		}
		else if (open_queue.begin()->get()->get_nodeid() == endID) {
			get_path_to_node(*open_queue.begin()->get(), path_coords);
			still_searching = false;
		}
		else {
			expand_top();
			found_to_open();
		}
	}
	return path_coords;
}