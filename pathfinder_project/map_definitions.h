//definitions of map values

//TODO come up with suitable scales for map->pixels
#ifndef DEFINITIONS_INCLUDED
#define DEFINITIONS_INCLUDED

#include <vector>
#include <Windows.h>
#include <map>
#include <string>

/*
0 - free space (white)
1 - obstacle (black)
2 - wall (black)
3 - start point (green)
4 - end point (red)
5 - path (blue)
*/

//color definitions
#ifndef color_white
#define color_white RGB(255,255,255)
#endif
#ifndef color_black
#define color_black RGB(0,0,0)
#endif
#ifndef color_green
#define color_green RGB(0,255,0)
#endif
#ifndef color_blue
#define color_blue RGB(0,0,255)
#endif
#ifndef color_red
#define color_red RGB(255,0,0)
#endif

//obstacle types integer values
const enum map_point_type { free_space, obstacle, wall, start_point, end_point, path };
//obstacle types color values - eg. pixel_defs(obstacle) will give color_black
const std::vector<COLORREF> pixel_defs{ color_white,color_black,color_black,color_green,color_red,color_blue };

//TODO remove this for the api version
const std::vector<std::string> console_defs{ " ","#","#","S","E","o" };
//translate map for int to pixel
const std::map<int, COLORREF> int_to_px_translate{
	std::make_pair(free_space,pixel_defs[free_space]),
	std::make_pair(obstacle,pixel_defs[obstacle]),
	std::make_pair(wall,pixel_defs[wall]),
	std::make_pair(start_point,pixel_defs[start_point]),
	std::make_pair(end_point,pixel_defs[end_point]),
	std::make_pair(path,pixel_defs[path])
};
#endif