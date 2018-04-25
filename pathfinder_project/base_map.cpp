//definitions for base_map class
#include "base_map.h"

/*
SHAPES
Small square - 2x2
Big square - 4x4
L-shape - 3(h)x2(w)
y=x mirrored L - 2(h)x3(w)
H line 1x5
V line 5x1
*/

//CONSTRUCTORS/DESTRUCTORS
base_map::base_map(const int height, const int width) {
	coordinates = std::move(array2D<map_point_type>(height + 2, width + 2, free_space));
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
	return generic_map_item::get_rows();
}
int base_map::get_cols()const {
	return generic_map_item::get_cols();
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
	return generic_map_item::operator()(i, j);
}
//SETTERS
void base_map::set_coord(const int i, const int j, const map_point_type point) {
	//TODO could add checking for end/start/walls etc
	generic_map_item::set_coord(i, j, point);
}

//SHAPE ADD
void base_map::add_shape(map_shape* shape_in) {
	find_start_end();
	/*iterate over the shape, checking whether any of the shape's non-free space points are start/end
	The iteration range is dependent on whether any of the shape hang outside the map*/
	int row_limit{ 0 };
	int col_limit{ 0 };
	if ((get_rows() - shape_in->get_anchor_i()) >= shape_in->get_rows()) {
		row_limit = shape_in->get_anchor_i() + shape_in->get_rows();
	}
	else {
		row_limit = get_rows();
	}
	if ((get_cols() - shape_in->get_anchor_j()) >= shape_in->get_cols()) {
		col_limit = shape_in->get_anchor_j() + shape_in->get_cols();
	}
	else {
		col_limit = get_cols();
	}
	//iterators for shape coordinates
	int shape_i{ 0 };
	for (int map_i{ shape_in->get_anchor_i() }; map_i < row_limit; map_i++) {
		int shape_j{ 0 };
		for (int map_j{ shape_in->get_anchor_j() }; map_j < col_limit; map_j++) {
			if ((shape_in->operator()(shape_i, shape_j) != free_space && operator()(map_i, map_j) == start_point) || 
				(shape_in->operator()(shape_i, shape_j) != free_space && operator()(map_i, map_j) == end_point)) {
				//if the point on the shape is not free, and the point in the map is start/end, end the function
				return;
			}
			shape_j++;
		}
		shape_i++;
	}
	//if not returned, continue with putting the shape's info into the map
	shape_i = 0;
	for (int map_i{ shape_in->get_anchor_i() }; map_i < row_limit; map_i++) {
		int shape_j{ 0 };
		for (int map_j{ shape_in->get_anchor_j() }; map_j < col_limit; map_j++) {
			//if the point in the map is a free space, set it as the corresponding point in the shape
			if (operator()(map_i, map_j) == free_space) {
				set_coord(map_i, map_j, shape_in->operator()(shape_i, shape_j));
			}
			shape_j++;
		}
		shape_i++;
	}
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