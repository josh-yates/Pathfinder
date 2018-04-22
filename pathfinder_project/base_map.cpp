//definitions for base_map class
#include "base_map.h"

//CONSTRUCTORS/DESTRUCTORS
base_map::base_map(const int height, const int width) {
	coordinates = std::move(array2D<map_point_type>(height, width, free_space));
	//set the walls
	for (int i{ 0 }; i < coordinates.get_m_rows(); i++) {
		coordinates.set_element(i, 0, wall);
		coordinates.set_element(i, coordinates.get_n_cols() - 1, wall);
	}
	for (int j{ 0 }; j < coordinates.get_n_cols(); j++) {
		coordinates.set_element(0, j, wall);
		coordinates.set_element(coordinates.get_m_rows() - 1, j, wall);
	}
	//set the start and end coordinates to not be in the map
	start_i = -1;
	start_j = -1;
	end_i = -1;
	end_j = -1;
}
base_map::~base_map() {}
//GETTERS
int base_map::get_rows()const {
	return coordinates.get_m_rows();
}
int base_map::get_cols()const {
	return coordinates.get_n_cols();
}
int base_map::get_start_i()const {
	return start_i;
}
int base_map::get_start_j()const {
	return start_j;
}
int base_map::get_end_i()const {
	return end_i;
}
int base_map::get_end_j()const {
	return end_j;
}
map_point_type base_map::operator()(const int i, const int j)const {
	return coordinates(i, j);
}
//SETTERS
void base_map::set_coord(const int i, const int j, const map_point_type point) {
	//TODO could add checking for end/start/walls etc
	generic_map_item::set_coord(i, j, point);
}

//FIND START AND END
void base_map::find_start_end() {
	//keep track of whether the points were found
	bool start_found{ false };
	bool end_found{ false };
	//coordinates to store temporarily found start and ends
	int temp_si{ -1 };
	int temp_sj{ -1 };
	int temp_ei{ -1 };
	int temp_ej{ -1 };
	//search every column and row
	for (int i{ 0 }; i < coordinates.get_m_rows(); i++) {
		for (int j{ 0 }; j < coordinates.get_n_cols(); j++) {
			//finding start points
			if (coordinates(i, j) == start_point && start_found == false) {
				temp_si = i;
				temp_sj = j;
				start_found = true;
			}
			else if (coordinates(i, j) == start_point && start_found == true) {
				throw "base_map start_find_end: two start points found";
			}
			//finding end points
			if (coordinates(i, j) == end_point && end_found == false) {
				temp_ei = i;
				temp_ej = j;
				end_found = true;
			}
			else if (coordinates(i, j) == end_point && end_found == true) {
				throw "base_map start_find_end: two end points found";
			}
		}
	}
	//if all worked fine, set the points to the temps
	start_i = temp_si;
	start_j = temp_sj;
	end_i = temp_ei;
	end_j = temp_ej;
}