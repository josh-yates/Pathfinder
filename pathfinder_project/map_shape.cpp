#include "map_shape.h"

//CONSTRUCTORS/DESTRUCTORS
map_shape::map_shape(base_map& map_in, const int anchor_i_in, const int anchor_j_in):anchor_i(anchor_i_in), anchor_j(anchor_j_in) {
	//range check inputs of anchor points
	if (anchor_i_in < 0 || anchor_i_in >= map_in.get_rows() || anchor_j_in < 0 || anchor_j_in >= map_in.get_cols()) {
		throw std::invalid_argument("map_shape constructor: anchor points out of range");
	}
}
map_shape::~map_shape(){}

//GETTERS
int map_shape::get_rows()const {
	return generic_map_item::get_rows();
}
int map_shape::get_cols()const {
	return generic_map_item::get_cols();
}
int map_shape::get_anchor_i()const {
	return anchor_i;
}
int map_shape::get_anchor_j()const {
	return anchor_j;
}
map_point_type map_shape::operator()(const int i, const int j)const {
	return generic_map_item::operator()(i, j);
}