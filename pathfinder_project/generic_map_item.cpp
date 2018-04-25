//definitions of functions in map_item class

#include "generic_map_item.h"

//VIRTUAL DESTRUCTOR
generic_map_item::~generic_map_item() {}
//GETTERS
int generic_map_item::get_rows()const {
	return coordinates.get_m_rows();
}
int generic_map_item::get_cols()const {
	return coordinates.get_n_cols();
}
map_point_type generic_map_item::operator()(const int i, const int j)const {
	return coordinates(i, j);
}
//SETTERS
void generic_map_item::set_coord(const int i, const int j, const map_point_type point) {
	//range checking
	if (i < 0 || i >= coordinates.get_m_rows() || j < 0 || j >= coordinates.get_n_cols()) {
		throw "map_item set_coord: element out of array range";
	}
	else {
		coordinates.set_element(i, j, point);
	}
}