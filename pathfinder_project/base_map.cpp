//definitions for base_map class
#include "base_map.h"

//CONSTRUCTORS/DESTRUCTORS
base_map::base_map(const int height, const int width) {
	coordinates = std::move(array2D<map_point_type>(height + 2, width + 2, free_space));
	//set the walls
	for (int i{ 0 }; i < coordinates.get_m_rows(); i++) {
		coordinates.set_element(i, 0, wall);
		coordinates.set_element(i, coordinates.get_n_cols() - 1, wall);
	}
	for (int j{ 0 }; j < coordinates.get_n_cols(); j++) {
		coordinates.set_element(0, j, wall);
		coordinates.set_element(coordinates.get_m_rows() - 1, j, wall);
	}
	//set the start and end coordinates to not be in the map
	start_i = -1;
	start_j = -1;
	end_i = -1;
	end_j = -1;
}
base_map::~base_map() {}
//GETTERS
int base_map::get_rows()const {
	return generic_map_item::get_rows();
}
int base_map::get_cols()const {
	return generic_map_item::get_cols();
}
int base_map::get_start_i()const {
	return start_i;
}
int base_map::get_start_j()const {
	return start_j;
}
int base_map::get_end_i()const {
	return end_i;
}
int base_map::get_end_j()const {
	return end_j;
}
map_point_type base_map::operator()(const int i, const int j)const {
	return generic_map_item::operator()(i, j);
}
//SETTERS
void base_map::set_coord(const int i, const int j, const map_point_type point) {
	//TODO could add checking for end/start/walls etc
	generic_map_item::set_coord(i, j, point);
}

//SHAPE ADD
void base_map::add_shape(map_shape* shape_in) {
	find_start_end();
	/*iterate over the shape, checking whether any of the shape's non-free space points are start/end
	The iteration range is dependent on whether any of the shape hang outside the map*/
	int row_limit{ 0 };
	int col_limit{ 0 };
	if ((get_rows() - shape_in->get_anchor_i()) >= shape_in->get_rows()) {
		row_limit = shape_in->get_anchor_i() + shape_in->get_rows();
	}
	else {
		row_limit = get_rows();
	}
	if ((get_cols() - shape_in->get_anchor_j()) >= shape_in->get_cols()) {
		col_limit = shape_in->get_anchor_j() + shape_in->get_cols();
	}
	else {
		col_limit = get_cols();
	}
	//iterators for shape coordinates
	int shape_i{ 0 };
	for (int map_i{ shape_in->get_anchor_i() }; map_i < row_limit; map_i++) {
		int shape_j{ 0 };
		for (int map_j{ shape_in->get_anchor_j() }; map_j < col_limit; map_j++) {
			if ((shape_in->operator()(shape_i, shape_j) != free_space && operator()(map_i, map_j) == start_point) || 
				(shape_in->operator()(shape_i, shape_j) != free_space && operator()(map_i, map_j) == end_point)) {
				//if the point on the shape is not free, and the point in the map is start/end, end the function
				return;
			}
			shape_j++;
		}
		shape_i++;
	}
	//if not returned, continue with putting the shape's info into the map
	shape_i = 0;
	for (int map_i{ shape_in->get_anchor_i() }; map_i < row_limit; map_i++) {
		int shape_j{ 0 };
		for (int map_j{ shape_in->get_anchor_j() }; map_j < col_limit; map_j++) {
			//if the point in the map is a free space, set it as the corresponding point in the shape
			if (operator()(map_i, map_j) == free_space) {
				set_coord(map_i, map_j, shape_in->operator()(shape_i, shape_j));
			}
			shape_j++;
		}
		shape_i++;
	}
}

//FIND START AND END
void base_map::find_start_end() {
	//keep track of whether the points were found
	bool start_found{ false };
	bool end_found{ false };
	//coordinates to store temporarily found start and ends
	int temp_si{ -1 };
	int temp_sj{ -1 };
	int temp_ei{ -1 };
	int temp_ej{ -1 };
	//search every column and row
	for (int i{ 0 }; i < coordinates.get_m_rows(); i++) {
		for (int j{ 0 }; j < coordinates.get_n_cols(); j++) {
			//finding start points
			if (coordinates(i, j) == start_point && start_found == false) {
				temp_si = i;
				temp_sj = j;
				start_found = true;
			}
			else if (coordinates(i, j) == start_point && start_found == true) {
				throw "base_map start_find_end: two start points found";
			}
			//finding end points
			if (coordinates(i, j) == end_point && end_found == false) {
				temp_ei = i;
				temp_ej = j;
				end_found = true;
			}
			else if (coordinates(i, j) == end_point && end_found == true) {
				throw "base_map start_find_end: two end points found";
			}
		}
	}
	//if all worked fine, set the points to the temps
	start_i = temp_si;
	start_j = temp_sj;
	end_i = temp_ei;
	end_j = temp_ej;
}

std::ostream& operator<<(std::ostream& os, const base_map& map_in) {
	//OUTPUT OF FORM  2 2 2\n2 0 2\n etc
	//iterate over rows and columns
	for (int i{ 0 }; i < map_in.get_rows(); i++) {
		for (int j{ 0 }; j < map_in.get_cols(); j++) {
			os << map_in(i, j);
			if (j < map_in.get_cols() - 1) {
				os << ',';
			}
		}
		os << std::endl;
	}
	return os;
}

std::istream& operator>>(std::istream& is, base_map& map_in) {
	//count number of rows and columns
	int RowCount{ 0 };
	int ColCount{ 0 };
	std::string LineDump;
	while (std::getline(is, LineDump)) {
		if (RowCount == 0) {
			ColCount = std::count(LineDump.begin(), LineDump.end(), ',');
		}
		else if (std::count(LineDump.begin(), LineDump.end(), ',') != ColCount) {
			//if the number of columns isn't equal
			OutputDebugString(L"Mismatching columns\n");
			throw std::invalid_argument("Mismatching columns");
		}
		RowCount++;
	}
	//correct column count, as there are col-1 spaces
	ColCount++;
	std::wstringstream rowcolstream;
	rowcolstream << RowCount << L" rows, " << ColCount << " cols" << std::endl;
	OutputDebugString(rowcolstream.str().c_str());
	//reset getline and create the map
	is.clear();
	is.seekg(0, is.beg);
	map_in = base_map(RowCount - 2, ColCount - 2);
	//iterate over the rows, getting each point's value
	bool StartFound{ false };		//make sure only 1 start and end present
	bool EndFound{ false };
	std::string Tile;
	std::string LineContents;
	std::stringstream LineStream;
	int i{ 0 };
	while (std::getline(is, LineContents)) {
		int j{ 0 };
		LineStream << LineContents;
		while (std::getline(LineStream, Tile, ',')) {
			map_in.set_coord(i, j, static_cast<map_point_type>(std::stoi(Tile)));
			j++;
		}
		LineStream.clear();
		i++;
	}
	return is;

	//for (int i{ 0 }; i < map_in.get_rows(); i++) {
	//	for (int j{ 0 }; j < map_in.get_cols(); j++) {
	//		std::getline(is, LineDump, ',');
	//		std::wstringstream errstream;
	//		errstream << i << L" " << j << " " << std::to_wstring(std::stoi(LineDump)) << std::endl;
	//		OutputDebugString(errstream.str().c_str());
	//		//if in a wall position, check it is a wall
	//		if ((i == 0 || i == map_in.get_rows() - 1 || j == 0 || j == map_in.get_cols() - 1) && (LineDump != std::to_string(wall))) {
	//			OutputDebugString(L"Invalid walls\n");
	//			throw std::invalid_argument("Invalid walls");
	//		}
	//		//next, check if it is a start/end and whether there's already a start/end
	//		else if ((LineDump == std::to_string(start_point) && StartFound) || (LineDump == std::to_string(end_point) && EndFound)) {
	//			OutputDebugString(L"Multiple start/end");
	//			throw std::invalid_argument("Multiple start/end");
	//		}
	//		//next, check if it is one of the map point types
	//		else if (LineDump != std::to_string(free_space) && LineDump != std::to_string(obstacle) 
	//			&& LineDump != std::to_string(wall) && LineDump != std::to_string(start_point) 
	//			&& LineDump != std::to_string(end_point) && LineDump != std::to_string(path)) {
	//			OutputDebugString(L"Not map_point_type\n");
	//			throw std::invalid_argument("Not map_point_type");
	//		}
	//		//check if start/end found
	//		else if (LineDump == std::to_string(start_point)) {
	//			StartFound = true;
	//		}
	//		else if (LineDump == std::to_string(end_point)) {
	//			EndFound = true;
	//		}
	//		//if point is path, overwrite with free space
	//		else if (LineDump == std::to_string(path)) {
	//			map_in.set_coord(i, j, free_space);
	//		}
	//		else {
	//			map_in.set_coord(i, j, static_cast<map_point_type>(std::stoi(LineDump)));
	//		}
	//	}
	//}
	return is;
}