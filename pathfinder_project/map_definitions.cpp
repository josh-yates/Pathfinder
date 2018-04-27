#include "map_definitions.h"

//obstacle types color values - eg. pixel_defs(obstacle) will give color_black
const std::vector<COLORREF> pixel_defs{ color_white,color_black,color_black,color_green,color_red,color_blue };

//translate map for int to pixel
const std::map<int, COLORREF> int_to_px_translate{
	std::make_pair(free_space,pixel_defs[free_space]),
	std::make_pair(obstacle,pixel_defs[obstacle]),
	std::make_pair(wall,pixel_defs[wall]),
	std::make_pair(start_point,pixel_defs[start_point]),
	std::make_pair(end_point,pixel_defs[end_point]),
	std::make_pair(path,pixel_defs[path])
};