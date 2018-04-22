//abstract class for generic map items (base map, obstacles etc)
#ifndef GENERIC_MAP_ITEM_INCLUDED
#define GENERIC_MAP_ITEM_INCLUDED

#include "array2D.h"
#include "map_definitions.h"

class generic_map_item {
protected:
	array2D<map_point_type> coordinates;
public:
	//VIRTUAL DESTRUCTOR
	//declared in .cpp, this makes map_item abstract
	virtual ~generic_map_item() = 0;
	//GETTERS
	int get_rows()const;
	int get_cols()const;
	//SETTERS
	void set_coord(const int, const int, const map_point_type);
};
#endif