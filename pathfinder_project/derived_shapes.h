#ifndef DERIVED_SHAPES_INCLUDED
#define DERIVED_SHAPES_INCLUDED

#include "map_shape.h"

//DERIVED MAP SHAPE CLASSES
class L_shape : public map_shape {
public:
	L_shape(base_map&, const int, const int);
	~L_shape();
};
class small_square :public map_shape {
public:
	small_square(base_map&, const int, const int);
	~small_square();
};
class big_square :public map_shape {
public:
	big_square(base_map&, const int, const int);
	~big_square();
};
class horizontal_line : public map_shape {
public:
	horizontal_line(base_map&, const int, const int);
	~horizontal_line();
};
class vertical_line : public map_shape {
public:
	vertical_line(base_map&, const int, const int);
	~vertical_line();
};
#endif