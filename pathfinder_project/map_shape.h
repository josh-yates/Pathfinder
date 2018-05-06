//abstract class for generic map shape

#ifndef MAP_SHAPE_INCLUDED
#define MAP_SHAPE_INCLUDED

#include <stdexcept>
#include "generic_map_item.h"
#include "base_map.h"

//make map_shape aware of base_map's existence
class base_map;

class map_shape :public generic_map_item {
private:
	/*coordinates for the anchor (top left corner) of the shape
	in the base map*/
	int anchor_i, anchor_j;
public:
	//CONSTRUCTORS/DESTRUCTORS
	map_shape(base_map&, const int, const int);
	virtual ~map_shape() = 0;
	//GETTERS
	int get_rows()const;
	int get_cols()const;
	int get_anchor_i()const;
	int get_anchor_j()const;
	map_point_type operator()(const int, const int)const;
};

#endif