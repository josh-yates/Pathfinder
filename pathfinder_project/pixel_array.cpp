//function definitions for pixel_array class
#include "pixel_array.h"
//CONSTRUCTORS/DESTRUCTORS
pixel_array::pixel_array(const base_map& map_in, const std::map<int, COLORREF> translation_key):pixel_data(map_in.get_rows(), map_in.get_cols(), default_color) {
	//translate each map_array item via translation_key's specifications
	for (int i{ 0 }; i < pixel_data.get_m_rows(); i++) {
		for (int j{ 0 }; j < pixel_data.get_n_cols(); j++) {
			//check ij'th element exists in translation_key
			const auto translation_iter = translation_key.find(map_in(i, j));
			if (translation_iter == translation_key.end()) {
				throw std::invalid_argument("pixel_array constructor: unable to translate from int");
			}
			else {
				pixel_data.set_element(i, j, translation_iter->second);
			}
		}
	}
}
pixel_array::~pixel_array(){}
//GETTERS
int pixel_array::get_rows()const {
	return pixel_data.get_m_rows();
}
int pixel_array::get_cols()const {
	return pixel_data.get_n_cols();
}
COLORREF pixel_array::operator()(const int i, const int j)const {
	return pixel_data(i, j);
}