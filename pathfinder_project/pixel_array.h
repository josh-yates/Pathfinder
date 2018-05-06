//class for 2D array of pixel color values
#ifndef PIXEL_ARRAY_INCLUDED
#define PIXEL_ARRAY_INCLUDED

#include "array2D.h"
#include <windows.h>
#include <map>
#include <stdexcept>
#include "base_map.h"

#ifndef default_color
#define default_color RGB(255,255,255)
#endif

class pixel_array {
private:
	//initially construct pixel_data
	array2D<COLORREF> pixel_data;
public:
	//CONSTRUCTORS/DESTRUCTORS
	pixel_array(const base_map&, const std::map<int, COLORREF>);
	~pixel_array();
	//GETTERS
	int get_rows()const;
	int get_cols()const;
	COLORREF operator()(const int, const int)const;
};
#endif