//abstract class for generic map items (base map, shapes etc)
#ifndef GENERIC_MAP_ITEM_INCLUDED
#define GENERIC_MAP_ITEM_INCLUDED

#include <stdexcept>
#include "array2D.h"
#include "map_definitions.h"

class generic_map_item {
protected:
	//2D array to store the map
	array2D<map_point_type> coordinates;
public:
	//VIRTUAL DESTRUCTOR
	//defined in .cpp, this makes generic_map_item abstract
	virtual ~generic_map_item() = 0;
	//GETTERS
	int get_rows()const;
	int get_cols()const;
	map_point_type operator()(const int, const int)const;
	//SETTERS
	void set_coord(const int, const int, const map_point_type);
};
#endif