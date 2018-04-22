//class for the base map that everything will be added to
//TODO add function for finding the start and end coordinates
//TODO add combination function for adding shapes, make this check for walls etc.
#ifndef BASE_MAP_INCLUDED
#define BASE_MAP_INCLUDED

#include "generic_map_item.h"
class base_map : public generic_map_item {
private:
	int start_i, start_j, end_i, end_j;
public:
	//CONSTRUCTORS/DESTRUCTORS
	base_map(const int, const int);
	~base_map();
	//GETTERS
	int get_rows()const;
	int get_cols()const;
	int get_start_i()const;
	int get_start_j()const;
	int get_end_i()const;
	int get_end_j()const;
	map_point_type operator()(const int, const int)const;
	//SETTERS
	void set_coord(const int, const int, const map_point_type);
	//FIND START AND END
	void find_start_end();
};
#endif