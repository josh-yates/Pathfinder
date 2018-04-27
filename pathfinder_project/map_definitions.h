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
#ifndef color_grey
#define color_grey RGB(100,100,100)
#endif

//obstacle types integer values
const enum map_point_type { free_space, obstacle, wall, start_point, end_point, path };
//obstacle types color values - eg. pixel_defs(obstacle) will give color_black
extern const std::vector<COLORREF> pixel_defs;
//translate map for int to pixel
extern const std::map<int, COLORREF> int_to_px_translate;
#endif