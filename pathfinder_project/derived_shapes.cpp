#include "derived_shapes.h"

L_shape::L_shape(base_map& map_in, const int anchor_i_in, const int anchor_j_in) :map_shape(map_in, anchor_i_in, anchor_j_in) {
	coordinates = std::move(array2D<map_point_type>(3, 2, free_space));
	//add side
	for (int i{ 0 }; i < coordinates.get_m_rows(); i++) {
		coordinates.set_element(i, 0, obstacle);
	}
	//add bottom
	for (int j{ 0 }; j < coordinates.get_n_cols(); j++) {
		coordinates.set_element(coordinates.get_m_rows() - 1, j, obstacle);
	}
}
L_shape::~L_shape(){}

//the rest of the shapes can be directly constructed as an array of obstacle points (don't contain any free spaces)
small_square::small_square(base_map& map_in, const int anchor_i_in, const int anchor_j_in) :map_shape(map_in, anchor_i_in, anchor_j_in) {
	coordinates = std::move(array2D<map_point_type>(2, 2, obstacle));
}
small_square::~small_square(){}

big_square::big_square(base_map& map_in, const int anchor_i_in, const int anchor_j_in) : map_shape(map_in, anchor_i_in, anchor_j_in) {
	coordinates = std::move(array2D<map_point_type>(4, 4, obstacle));
}
big_square::~big_square(){}

horizontal_line::horizontal_line(base_map& map_in, const int anchor_i_in, const int anchor_j_in) : map_shape(map_in, anchor_i_in, anchor_j_in) {
	coordinates = std::move(array2D<map_point_type>(1, 5, obstacle));
}
horizontal_line::~horizontal_line(){}

vertical_line::vertical_line(base_map& map_in, const int anchor_i_in, const int anchor_j_in) : map_shape(map_in, anchor_i_in, anchor_j_in) {
	coordinates = std::move(array2D<map_point_type>(5, 1, obstacle));
}
vertical_line::~vertical_line(){}