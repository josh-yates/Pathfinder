//MAIN PROGRAM FILE FOR PATHFINDER PROJECT

#define NOMINMAX		//handles the clash between windows.h min/max and limits min/max

//INCLUDES
#include <Windows.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <limits>
#include <cmath>
#include "base_map.h"
#include "astar.h"
#include "pixel_array.h"
#include "derived_shapes.h"

//CUSTOM MESSAGE DEFINITIONS
#define START_SCREEN__OPEN 1
#define START_SCREEN__NEW 2
#define NEW_MAP_SCREEN_CREATE 3
#define MAP_EDIT_SCREEN_EXIT 4
#define MAP_EDIT_SCREEN_RESTART 5
#define MAP_EDIT_SCREEN_ADD_POINTS_OBSTACLE 6
#define MAP_EDIT_SCREEN_ADD_POINTS_START 7
#define MAP_EDIT_SCREEN_ADD_POINTS_END 8
#define MAP_EDIT_RUN_ALGORITHM 9
#define MAP_EDIT_SCREEN_ADD_L_SHAPE 10
#define MAP_EDIT_SCREEN_ADD_SMALL_SQUARE 11
#define MAP_EDIT_SCREEN_ADD_FREE_SPACE 12
#define MAP_EDIT_SCREEN_ADD_BIG_SQUARE 13
#define	MAP_EDIT_SCREEN_ADD_V_LINE 14
#define MAP_EDIT_SCREEN_ADD_H_LINE 15
#define MAP_EDIT_SCREEN_HELP 16

//MAP RELATED VARIABLES
const int MinMapSize{ 2 };
const int MaxMapSize{ 50 };
const int MaxBitmapSize{ 500 };

//PARENT WINDOW VARIABLES
int ParentHeight{ 200 };
int ParentWidth{ 300 };
std::vector<HWND*> ChildWindowPtrs;
std::vector<HMENU*> MenuPtrs;
bool MapIsShowing{ false };
bool AlgorithmRun{ false };

//FORWARD DECLARATIONS
LRESULT CALLBACK window_procedure(HWND, UINT, WPARAM, LPARAM);			//message handler
void GetWindowSize(HWND, int&, int&);
void DeleteWindowContents(HWND);
bool CheckTextPosInt(HWND, const wchar_t*, const int, const int, int&);	//for checking text input is an integer
int CalculateScale(const int, const int);
void FalseAllMapEdits();
void ShowHelpDialog(HWND);

void DisplayStartScreen(HWND);											//start screen, with a message and two buttons
HWND hStartMessage;
HWND hOpenButton;
HWND hNewButton;

void DisplayNewMapScreen(HWND);											//screen shown when new map is to be created
HWND hNewMapInstructions;
HWND hNewMapHeightLabel;
HWND hNewMapHeightInput;
HWND hNewMapWidthLabel;
HWND hNewMapWidthInput;
HWND hNewMapCreateButton;

void DisplayMapEditScreen(HWND, const int, const int);					//screen shown when map is being edited
base_map UserMap(4, 4);
HMENU hMapEditMenu;
HWND hBitmapHolder;
HWND hStatusBar;
bool MapEditAddingFreeSpace{ false };
bool MapEditAddingObstacle{ false };
bool MapEditAddingStart{ false };
bool MapEditAddingEnd{ false };
bool MapEditAddingL_Shape{ false };
bool MapEditAddingSmallSquare{ false };
bool MapEditAddingBigSquare{ false };
bool MapEditAddingVLine{ false };
bool MapEditAddingHLine{ false };

void DisplayUserMap(HDC, const pixel_array&, const int);				//function for displaying user map as bitmap on screen


//MAIN PROGRAM
int WINAPI WinMain(
	HINSTANCE hInst,
	HINSTANCE hPrevInst,
	LPSTR args,
	int ncmdshow) {

	//CREATE WINDOW CLASS
	WNDCLASSW main_window_class{ 0 };
	main_window_class.hbrBackground = (HBRUSH)COLOR_WINDOW;		//paint background colour
	main_window_class.hCursor = LoadCursor(NULL, IDC_ARROW);	//set cursor type
	main_window_class.hInstance = hInst;						//current instance of this window, provided by OS
	main_window_class.lpszClassName = L"main_window_class";		//window class identifier
	main_window_class.lpfnWndProc = window_procedure;			//provide a message handler

	//REGISTER WINDOW CLASS WITH OS
	if (!RegisterClassW(&main_window_class)) {
		MessageBox(NULL, L"Failed to register main_window_class", L"Error", MB_ICONWARNING);
	}

	//CREATE WINDOW OBJECT
	CreateWindowW(
		L"main_window_class",		//the window class to make this object part of
		L"Pathfinder",				//window title
		WS_SYSMENU | WS_VISIBLE,	//window display properties (non-resizeable, visible)
		100, 100,					//x,y position of top LH corner on screen
		ParentWidth, ParentHeight,					//x,y dimensions of window
		NULL, NULL, NULL, NULL);

	//CREATE MESSAGE LOOP
	MSG message{ 0 };		//object to store messages
	while (GetMessage(&message, NULL, NULL, NULL)) {
		TranslateMessage(&message);
		DispatchMessage(&message);		//translate message and send to window procedure
	}

	return 0;
}

//DECLARATION DEFINITIONS
LRESULT CALLBACK window_procedure(HWND hWnd, UINT message, WPARAM wp, LPARAM lp) {
	//case dependent message handler
	switch (message) {
	//WINDOW CREATION
	case WM_CREATE:
		DisplayStartScreen(hWnd);
		//TODO add functions here for displaying the buttons on creation
		break;
	//COMMANDS FROM ITEMS IN WINDOW
	case WM_COMMAND:
		//TODO add functions here for handling messages from the buttons
		switch (wp) {
		case START_SCREEN__NEW:
			DisplayNewMapScreen(hWnd);
			break;
		case NEW_MAP_SCREEN_CREATE: {
			//check height and width inputs, and get integer values if correct
			wchar_t height_in[100];
			wchar_t width_in[100];
			int height_out{ 0 };
			int width_out{ 0 };
			//collect input from text boxes
			GetWindowTextW(hNewMapHeightInput, height_in, 100);
			GetWindowTextW(hNewMapWidthInput, width_in, 100);
			if (CheckTextPosInt(hWnd, height_in, MinMapSize, MaxMapSize, height_out) && 
				CheckTextPosInt(hWnd, width_in, MinMapSize, MaxMapSize, width_out)) {
				//open map editor
				DisplayMapEditScreen(hWnd, height_out, width_out);
			}
			break; 
		}

		//MAP EDIT FILE (ETC) MESSAGES
		case MAP_EDIT_SCREEN_EXIT:
			//call the wm_destroy message to close the window
			SendMessage(hWnd, WM_DESTROY, NULL, NULL);
			break;
		case MAP_EDIT_SCREEN_RESTART:
			DisplayStartScreen(hWnd);
			break;
		case MAP_EDIT_SCREEN_HELP:
			ShowHelpDialog(hWnd);
			break;

		//POINTS MESSAGES

		case MAP_EDIT_SCREEN_ADD_POINTS_START:
			//check start not already defined
			UserMap.find_start_end();
			if (UserMap.get_start_i() < 0 && UserMap.get_start_j() < 0) {
				FalseAllMapEdits();
				MapEditAddingStart = true;
			}
			else if (AlgorithmRun) {
				FalseAllMapEdits();
				MessageBox(hBitmapHolder, L"Algorithm already run", L"Algorithm result", MB_ICONINFORMATION);
				MapEditAddingStart = false;
			}
			else {
				FalseAllMapEdits();
				MessageBox(hBitmapHolder, L"Start point already defined", L"Warning", MB_ICONWARNING);
				MapEditAddingStart = false;
			}
			break;
		case MAP_EDIT_SCREEN_ADD_POINTS_END:
			//check end not already defined
			UserMap.find_start_end();
			if (UserMap.get_end_i() < 0 && UserMap.get_end_j() < 0) {
				FalseAllMapEdits();
				MapEditAddingEnd = true;
			}
			else if (AlgorithmRun) {
				FalseAllMapEdits();
				MessageBox(hBitmapHolder, L"Algorithm already run", L"Algorithm result", MB_ICONINFORMATION);
				MapEditAddingEnd = false;
			}
			else {
				FalseAllMapEdits();
				MessageBox(hBitmapHolder, L"End point already defined", L"Warning", MB_ICONWARNING);
				MapEditAddingEnd = false;
			}
			break;
		case MAP_EDIT_SCREEN_ADD_POINTS_OBSTACLE:
			if (AlgorithmRun) {
				MessageBox(hBitmapHolder, L"Algorithm already run", L"Algorithm result", MB_ICONINFORMATION);
				MapEditAddingObstacle = false;
			}
			//toggle the obstacle adding
			else if (MapEditAddingObstacle) {
				FalseAllMapEdits();
			}
			else if (!MapEditAddingObstacle) {
				FalseAllMapEdits();
				MapEditAddingObstacle = true;
			}
			break;
		case MAP_EDIT_SCREEN_ADD_FREE_SPACE:
			if (AlgorithmRun) {
				MessageBox(hBitmapHolder, L"Algorithm already run", L"Algorithm result", MB_ICONINFORMATION);
				MapEditAddingFreeSpace = false;
			}
			//toggle the obstacle adding
			else if (MapEditAddingFreeSpace) {
				FalseAllMapEdits();
			}
			else if (!MapEditAddingFreeSpace) {
				FalseAllMapEdits();
				MapEditAddingFreeSpace = true;
			}
			break;

		//SHAPE MESSAGES

		case MAP_EDIT_SCREEN_ADD_L_SHAPE:
			if (AlgorithmRun) {
				MessageBox(hBitmapHolder, L"Algorithm already run", L"Algorithm result", MB_ICONINFORMATION);
				MapEditAddingL_Shape = false;
			}
			//toggle the shape adding
			else if (MapEditAddingL_Shape) {
				FalseAllMapEdits();
			}
			else if (!MapEditAddingL_Shape) {
				FalseAllMapEdits();
				MapEditAddingL_Shape = true;
			}
			break;
		case MAP_EDIT_SCREEN_ADD_SMALL_SQUARE:
			if (AlgorithmRun) {
				MessageBox(hBitmapHolder, L"Algorithm already run", L"Algorithm result", MB_ICONINFORMATION);
				MapEditAddingSmallSquare = false;
			}
			//toggle the shape adding
			else if (MapEditAddingSmallSquare) {
				FalseAllMapEdits();
			}
			else if (!MapEditAddingSmallSquare) {
				FalseAllMapEdits();
				MapEditAddingSmallSquare = true;
			}
			break;
		case MAP_EDIT_SCREEN_ADD_BIG_SQUARE:
			if (AlgorithmRun) {
				MessageBox(hBitmapHolder, L"Algorithm already run", L"Algorithm result", MB_ICONINFORMATION);
				MapEditAddingBigSquare = false;
			}
			//toggle the shape adding
			else if (MapEditAddingBigSquare) {
				FalseAllMapEdits();
			}
			else if (!MapEditAddingBigSquare) {
				FalseAllMapEdits();
				MapEditAddingBigSquare = true;
			}
			break;
		//LINES MESSAGES
		case MAP_EDIT_SCREEN_ADD_H_LINE:
			if (AlgorithmRun) {
				MessageBox(hBitmapHolder, L"Algorithm already run", L"Algorithm result", MB_ICONINFORMATION);
				MapEditAddingHLine = false;
			}
			//toggle the shape adding
			else if (MapEditAddingHLine) {
				FalseAllMapEdits();
			}
			else if (!MapEditAddingHLine) {
				FalseAllMapEdits();
				MapEditAddingHLine = true;
			}
			break;
		case MAP_EDIT_SCREEN_ADD_V_LINE:
			if (AlgorithmRun) {
				MessageBox(hBitmapHolder, L"Algorithm already run", L"Algorithm result", MB_ICONINFORMATION);
				MapEditAddingVLine = false;
			}
			//toggle the shape adding
			else if (MapEditAddingVLine) {
				FalseAllMapEdits();
			}
			else if (!MapEditAddingVLine) {
				FalseAllMapEdits();
				MapEditAddingVLine = true;
			}
			break;

		//ALGORITHM MESSAGE

		case MAP_EDIT_RUN_ALGORITHM: {
			FalseAllMapEdits();
			//check if start/end not defined
			UserMap.find_start_end();
			if (UserMap.get_start_i() < 0 || UserMap.get_start_j() < 0) {
				MessageBox(hBitmapHolder, L"Start point not defined", L"Warning", MB_ICONWARNING);
			}
			else if (UserMap.get_end_i() < 0 || UserMap.get_end_j() < 0) {
				MessageBox(hBitmapHolder, L"End point not defined", L"Warning", MB_ICONWARNING);
			}
			else if (AlgorithmRun) {
				MessageBox(hBitmapHolder, L"Algorithm already run", L"Algorithm result", MB_ICONINFORMATION);
			}
			else {
				//else, run the algorithm and update the map with the path coordinates
				astar run_algo(UserMap);
				std::vector<std::vector<int>> path_coords{ run_algo.find_path() };
				if (path_coords.size() == 0) {
					MessageBox(hBitmapHolder, L"No path found :(", L"Algorithm result", MB_ICONINFORMATION);
				}
				for (auto coord = path_coords.begin(); coord != path_coords.end(); coord++) {
					if (UserMap(coord->at(0), coord->at(1)) != start_point && UserMap(coord->at(0), coord->at(1)) != end_point) {
						UserMap.set_coord(coord->at(0), coord->at(1), path);
					}
				}
				AlgorithmRun = true;
			}
			InvalidateRect(hWnd, NULL, FALSE);
		}
		}
		break;
	//WINDOW CLOSURE
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
	//BITMAP PAINTING
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc;
		if (MapIsShowing) {
			hdc = BeginPaint(hBitmapHolder, &ps);
			pixel_array px_arr(UserMap, int_to_px_translate);
			int Scale{ CalculateScale(UserMap.get_cols(),MaxBitmapSize) };
			DisplayUserMap(hdc, px_arr, Scale);
			EndPaint(hBitmapHolder, &ps);
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		else {
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break; 
	}
	//LEFT MOUSE CLICK
	case WM_LBUTTONDOWN: {
		if (MapIsShowing && !AlgorithmRun) {
			//store mouse position in structure
			POINT MousePos;
			MousePos.x = LOWORD(lp);
			MousePos.y = HIWORD(lp);
			//calculate scale used to get to bitmap size
			int Scale{ CalculateScale(UserMap.get_cols(),MaxBitmapSize) };
			int MapXClick{ static_cast<int>(static_cast<double>(MousePos.x) / static_cast<double>(Scale)) };
			int MapYClick{ static_cast<int>(static_cast<double>(MousePos.y) / static_cast<double>(Scale)) };
			//check point is inside the user map
			if (MapXClick >= 0 && MapXClick < UserMap.get_cols() && MapYClick >= 0 && MapYClick < UserMap.get_rows()) {
				//POINT HANDLERS
				if (MapEditAddingObstacle || MapEditAddingEnd || MapEditAddingStart || MapEditAddingFreeSpace) {

					//if the clicked area is in the map, add the corresponding point type
					map_point_type point_to_add{ free_space };
					if (MapEditAddingObstacle) {
						FalseAllMapEdits();
						MapEditAddingObstacle = true;		//keep obstacle point toggled on
						point_to_add = obstacle;
					}
					else if (MapEditAddingEnd) {
						FalseAllMapEdits();
						point_to_add = end_point;
					}
					else if (MapEditAddingStart) {
						FalseAllMapEdits();
						point_to_add = start_point;
					}
					else if (MapEditAddingFreeSpace) {
						FalseAllMapEdits();
						MapEditAddingFreeSpace = true;		//keep free space toggled on
					}
					//if adding a free space, check that the click spot is not a wall, else, check it's a free space
					if (MapEditAddingFreeSpace && UserMap(MapYClick, MapXClick) != wall) {
						UserMap.set_coord(MapYClick, MapXClick, free_space);
					}
					else if (UserMap(MapYClick, MapXClick) == free_space) {
						UserMap.set_coord(MapYClick, MapXClick, point_to_add);
					}
				}
				//SHAPE/LINE HANDLERS
				else if (MapEditAddingL_Shape || MapEditAddingSmallSquare || MapEditAddingBigSquare || MapEditAddingHLine || MapEditAddingVLine) {
					//depending on shape/line type (lines are shapes), add that shape to the map
					if (MapEditAddingL_Shape) {
						FalseAllMapEdits();
						MapEditAddingL_Shape = true;
						L_shape lshape(UserMap, MapYClick, MapXClick);
						UserMap.add_shape(&lshape);
					}
					else if (MapEditAddingSmallSquare) {
						FalseAllMapEdits();
						MapEditAddingSmallSquare = true;
						small_square ssquare(UserMap, MapYClick, MapXClick);
						UserMap.add_shape(&ssquare);
					}
					else if (MapEditAddingBigSquare) {
						FalseAllMapEdits();
						MapEditAddingBigSquare = true;
						big_square bsquare(UserMap, MapYClick, MapXClick);
						UserMap.add_shape(&bsquare);
					}
					else if (MapEditAddingHLine) {
						FalseAllMapEdits();
						MapEditAddingHLine = true;
						horizontal_line hline(UserMap, MapYClick, MapXClick);
						UserMap.add_shape(&hline);
					}
					else if (MapEditAddingVLine) {
						FalseAllMapEdits();
						MapEditAddingVLine = true;
						vertical_line vline(UserMap, MapYClick, MapXClick);
						UserMap.add_shape(&vline);
					}
				}
			}
		}
		//force repaint
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	//DEFAULT PROCEDURE
	default:
		return DefWindowProcW(hWnd, message, wp, lp);
		break;
	}
}

void GetWindowSize(HWND hWnd, int& width, int& height) {
	RECT WindowRect;
	if (!GetWindowRect(hWnd, &WindowRect)) {
		//break and tell user
		MessageBox(NULL, L"Unable to access window RECT", L"Error", MB_ICONERROR);
		return;
	}
	width = WindowRect.right - WindowRect.left;
	height = WindowRect.bottom - WindowRect.top;
}

void DeleteWindowContents(HWND hWnd) {
	//map is not showing, don't allow "carry over" of obstacle input info
	MapIsShowing = false;
	FalseAllMapEdits();
	AlgorithmRun = false;
	//iterate over child windows, destroying them all
	for (auto child = ChildWindowPtrs.begin(); child != ChildWindowPtrs.end(); child++) {
		DestroyWindow(**child);
	}
	//clear the vector
	ChildWindowPtrs.clear();
	//clear the menus
	SetMenu(hWnd, NULL);
	for (auto menu = MenuPtrs.begin(); menu != MenuPtrs.end(); menu++) {
		DestroyMenu(**menu);
	}
	MenuPtrs.clear();
}

void FalseAllMapEdits() {
	//set all map editing trackers to false
	MapEditAddingFreeSpace = false;
	MapEditAddingL_Shape = false;
	MapEditAddingObstacle = false;
	MapEditAddingSmallSquare = false;
	MapEditAddingStart = false;
	MapEditAddingEnd = false;
	MapEditAddingBigSquare = false;
	MapEditAddingVLine = false;
	MapEditAddingHLine = false;
}

bool CheckTextPosInt(HWND hWnd, const wchar_t* input_wchar,const int min_val, const int max_val, int& int_out) {
	//CONVERT WCHAR_T TO WSTRING
	std::wstring input_string{ input_wchar };
	//remove end of line and spaces with wstringstreams
	std::wstringstream remove_eol(input_string);
	wchar_t buff_peek;
	while ((buff_peek = static_cast<wchar_t>(remove_eol.peek())) == L' ') {
		remove_eol.ignore(1, L'\n');
	}
	//remove the end of line, if present, then put it back on
	std::getline(remove_eol, input_string);
	std::wstringstream input_stream(input_string + L'\n');
	double input_double;
	//try to put stream into the double
	input_stream >> input_double;
	//check if it fails, if so, clear the failbit and empty the buffer
	if (input_stream.fail()) {
		input_stream.clear(); input_stream.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
		MessageBox(hWnd, L"Inputs must be integer numbers", L"Invalid input", MB_ICONERROR);
		return false;
	}
	//else, if anything followed the number, fail
	else if ((buff_peek = static_cast<wchar_t>(input_stream.peek())) != L'\n') {
		input_stream.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
		MessageBox(hWnd, L"Inputs must be integer numbers", L"Invalid input", MB_ICONERROR);
		return false;
	}
	//else, it is a number, now check it is >= min_val & is an integer
	else if (input_double < min_val || input_double>max_val) {
		std::wstring range_msg{ L"Inputs must be in range " + std::to_wstring(min_val) + L" to " + std::to_wstring(max_val) };
		MessageBox(hWnd, range_msg.c_str(), L"Invalid input", MB_ICONERROR);
		return false;
	}
	else {
		double int_part, fract_part;
		fract_part = modf(input_double, &int_part);
		if (fract_part == 0) {
			//it is an integer, convert and pass out
			int_out = static_cast<int>(int_part);
			return true;
		}
		else {
			MessageBox(hWnd, L"Inputs must be integer numbers", L"Invalid input", MB_ICONERROR);
			return false;
		}
	}
}

int CalculateScale(const int int_in, const int max_size) {
	//TARGET - ~400px window size
	double double_scale{ static_cast<double>(max_size) / static_cast<double>(int_in) };
	double rounded_double{ round(double_scale) };
	return static_cast<int>(rounded_double);
}

void ShowHelpDialog(HWND hWnd) {

	//TODO - add help button to start screen

	//create help/instruction message
	std::wstringstream HelpStream;
	HelpStream << L"PATHFINDER HELP PAGE" << std::endl << std::endl;
	HelpStream << L"Pathfinder.exe is a program for creating grid-based maps and finding routes through them." << std::endl << std::endl;
	HelpStream << L"START SCREEN:" << std::endl;
	HelpStream << L"You can load a map from a file for editing, or create a new map on the new map screen. "
		<< L"If the loaded map is already solved, the route will be cleared" << std::endl << std::endl;
	HelpStream << L"NEW MAP SCREEN:" << std::endl;
	HelpStream << L"You can enter the width and height values for the map, between " << MinMapSize << L" and " << MaxMapSize << L" tiles."
		<< L" Walls are automatically added outside the map space. If the width and height are not equal, the map edge size is the bigger of the two"
		<< L" and walls are added to reach the desired dimensions." << std::endl << std::endl;
	HelpStream << L"MAP EDIT SCREEN:" << std::endl;
	HelpStream << L"Here you can add obstacles and the start and end points to create your map. The controls for all obstacles (point, line shape) are toggleable, "
		<< L"so you can use the same obstacle multiple times without needing to visit the menu. Switching to another obstacle will toggle-off the previous one. "
		<< L" Free spaces are in the points menu, and can be used to remove obstacle/start/end points. The start and end points are not toggleable and can "
		<< L"only be placed once, unless the original is removed with a free space. The path can be found by clicking Run, but the algorithm will only run when both the start and end points are present. "
		<< L"If no route can be found a notification appears. The algorithm is quick, but there may be a noticeable delay on large maps. The map is not editable after "
		<< L"the algorithm has been run. If the map is saved after the algorithm is run, it will contain details of the route found.";
	MessageBox(hWnd, HelpStream.str().c_str(), L"Help page", NULL);
}

//SCREEN FUNCTIONS

void DisplayStartScreen(HWND hWnd) {
	//CLEAR CHILDREN WINDOWS SET PARENT WINDOW DIMENSIONS AND RESIZE
	DeleteWindowContents(hWnd);
	ParentHeight = 200;
	ParentWidth = 300;
	SetWindowPos(hWnd, NULL, NULL, NULL, ParentWidth, ParentHeight, SWP_NOMOVE);

	//START MESSAGE
	int StartMessageWidth{ 180 };
	int StartMessageHeight{ 40 };
	int StartMessageXPos{ (ParentWidth - StartMessageWidth) / 2 };
	int StartMessageYpos{ 10 };
	hStartMessage = CreateWindowW(L"Static", L"Welcome to Pathfinder.exe\nChoose an option:", WS_VISIBLE | WS_CHILD | SS_CENTER,
		StartMessageXPos, StartMessageYpos, StartMessageWidth, StartMessageHeight, hWnd, NULL, NULL, NULL);

	//OPEN FILE BUTTON
	int OpenButtonWidth{ 110 };
	int OpenButtonHeight{ 30 };
	int OpenButtonXPos{ (ParentWidth - OpenButtonWidth) / 2 };
	int OpenButtonYPos{ StartMessageYpos + StartMessageHeight + 10 };
	hOpenButton = CreateWindowW(L"Button", L"Open from file", WS_VISIBLE | WS_CHILD | SS_CENTER,
		OpenButtonXPos, OpenButtonYPos, OpenButtonWidth, OpenButtonHeight, hWnd, NULL, NULL, NULL);

	//NEW MAP BUTTON
	int NewButtonWidth{ 110 };
	int NewButtonHeight{ 30 };
	int NewButtonXPos{ (ParentWidth - NewButtonWidth) / 2 };
	int NewButtonYPos{ OpenButtonYPos + NewButtonHeight + 10 };
	hNewButton = CreateWindowW(L"Button", L"Create new map", WS_VISIBLE | WS_CHILD | SS_CENTER,
		NewButtonXPos, NewButtonYPos, NewButtonWidth, NewButtonHeight, hWnd, (HMENU)START_SCREEN__NEW, NULL, NULL);

	//TODO add messages for the two buttons
	ChildWindowPtrs.push_back(&hStartMessage);
	ChildWindowPtrs.push_back(&hOpenButton);
	ChildWindowPtrs.push_back(&hNewButton);
}

void DisplayNewMapScreen(HWND hWnd) {
	//CLEAR THE WINDOW AND RESIZE
	DeleteWindowContents(hWnd);
	ParentHeight = 250;
	ParentWidth = 300;
	SetWindowPos(hWnd, NULL, NULL, NULL, ParentWidth, ParentHeight, SWP_NOMOVE);		//resize the window, without moving it

	//INSTRUCTIONS
	int InstructionsWidth{ 200 };
	int InstructionsHeight{ 50 };
	int InstructionsXPos{ (ParentWidth - InstructionsWidth) / 2 };
	int InstructionsYPos{ 10 };
	hNewMapInstructions = CreateWindowW(L"Static", L"Insert width and height\n for the new map:", WS_VISIBLE | WS_CHILD | SS_CENTER,
		InstructionsXPos, InstructionsYPos, InstructionsWidth, InstructionsHeight, hWnd, NULL, NULL, NULL);

	//WIDTH INPUT BOX AND LABEL
	int WidthLabelWidth{ 100 };
	int WidthLabelHeight{ 20 };
	int WidthInputWidth{ 100 };
	int WidthInputHeight{ 20 };
	int WidthLabelXPos{ (ParentWidth - WidthInputWidth - WidthLabelWidth) / 2 };
	int WidthLabelYPos{ InstructionsYPos + InstructionsHeight + 10 };
	int WidthInputXPos{ WidthLabelXPos + WidthLabelWidth };
	int WidthInputYpos{ WidthLabelYPos };
	hNewMapWidthLabel = CreateWindowW(L"Static", L"Width:", WS_VISIBLE | WS_CHILD | SS_CENTER,
		WidthLabelXPos, WidthLabelYPos, WidthLabelWidth, WidthLabelHeight, hWnd, NULL, NULL, NULL);
	hNewMapWidthInput = CreateWindowW(L"Edit", L"10", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOVSCROLL |SS_CENTER,
		WidthInputXPos, WidthInputYpos, WidthInputWidth, WidthInputHeight, hWnd, NULL, NULL, NULL);

	//HEIGHT INPUT BOX AND LABEL
	int HeightLabelWidth{ 100 };
	int HeightLabelHeight{ 20 };
	int HeightInputWidth{ 100 };
	int HeightInputHeight{ 20 };
	int HeightLabelXPos{ (ParentWidth - HeightInputWidth - HeightLabelWidth) / 2 };
	int HeightLabelYPos{ WidthLabelYPos + WidthLabelHeight + 10 };
	int HeightInputXPos{ HeightLabelXPos + HeightLabelWidth };
	int HeightInputYpos{ HeightLabelYPos };
	hNewMapHeightLabel = CreateWindowW(L"Static", L"Height:", WS_VISIBLE | WS_CHILD | SS_CENTER,
		HeightLabelXPos, HeightLabelYPos, HeightLabelWidth, HeightLabelHeight, hWnd, NULL, NULL, NULL);
	hNewMapHeightInput = CreateWindowW(L"Edit", L"10", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOVSCROLL | SS_CENTER,
		HeightInputXPos, HeightInputYpos, HeightInputWidth, HeightInputHeight, hWnd, NULL, NULL, NULL);

	//CREATE BUTTON
	int CreateButtonWidth{ 100 };
	int CreateButtonHeight{ 30 };
	int CreateButtonXPos{ (ParentWidth - CreateButtonWidth) / 2 };
	int CreateButtonYPos{ HeightLabelYPos + HeightLabelHeight + 20 };
	hNewMapCreateButton = CreateWindowW(L"Button", L"Create", WS_VISIBLE | WS_CHILD | SS_CENTER,
		CreateButtonXPos, CreateButtonYPos, CreateButtonWidth, CreateButtonHeight, hWnd, (HMENU)NEW_MAP_SCREEN_CREATE, NULL, NULL);

	//TODO add message for create button

	ChildWindowPtrs.push_back(&hNewMapInstructions);
	ChildWindowPtrs.push_back(&hNewMapWidthLabel);
	ChildWindowPtrs.push_back(&hNewMapWidthInput);
	ChildWindowPtrs.push_back(&hNewMapHeightLabel);
	ChildWindowPtrs.push_back(&hNewMapHeightInput);
	ChildWindowPtrs.push_back(&hNewMapCreateButton);
}

void DisplayMapEditScreen(HWND hWnd, const int MapHeight, const int MapWidth){
	//CHOOSE WHICH TO MAKE THE LONG SIDE
	int BmpSideSize;
	if (MapHeight > MapWidth) {
		BmpSideSize = MapHeight;
	}
	else {
		BmpSideSize = MapWidth;
	}

	//MENUS

	//CLEAR WINDOW
	DeleteWindowContents(hWnd);

	//ADD MAIN MENU
	hMapEditMenu = CreateMenu();

	//ADD FILE MENU
	//TODO add messages for menu buttons
	HMENU hFileMenu{ CreateMenu() };
	AppendMenu(hFileMenu, MF_STRING, MAP_EDIT_SCREEN_RESTART, L"Restart");
	AppendMenu(hFileMenu, MF_STRING, NULL, L"Save Map");
	AppendMenu(hFileMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hFileMenu, MF_STRING, MAP_EDIT_SCREEN_EXIT, L"Exit");

	AppendMenu(hMapEditMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");

	//ADD SHAPES MENU
	HMENU hShapesMenu{ CreateMenu() };
	AppendMenu(hShapesMenu, MF_STRING, MAP_EDIT_SCREEN_ADD_SMALL_SQUARE, L"Small square");
	AppendMenu(hShapesMenu, MF_STRING, MAP_EDIT_SCREEN_ADD_BIG_SQUARE, L"Big square");
	AppendMenu(hShapesMenu, MF_STRING, MAP_EDIT_SCREEN_ADD_L_SHAPE, L"L shape");

	//ADD POINTS MENU
	HMENU hPointsMenu{ CreateMenu() };
	AppendMenu(hPointsMenu, MF_STRING, MAP_EDIT_SCREEN_ADD_POINTS_START, L"Start");
	AppendMenu(hPointsMenu, MF_STRING, MAP_EDIT_SCREEN_ADD_POINTS_END, L"End");
	AppendMenu(hPointsMenu, MF_STRING, MAP_EDIT_SCREEN_ADD_FREE_SPACE, L"Free space");
	AppendMenu(hPointsMenu, MF_STRING, MAP_EDIT_SCREEN_ADD_POINTS_OBSTACLE, L"Obstacle");

	//ADD LINES MENU
	HMENU hLinesMenu{ CreateMenu() };
	AppendMenu(hLinesMenu, MF_STRING, MAP_EDIT_SCREEN_ADD_V_LINE, L"Vertical");
	AppendMenu(hLinesMenu, MF_STRING, MAP_EDIT_SCREEN_ADD_H_LINE, L"Horizontal");

	//ADD INSERT MENU
	HMENU hInsertMenu{ CreateMenu() };
	AppendMenu(hInsertMenu, MF_POPUP, (UINT_PTR)hPointsMenu, L"Points");
	AppendMenu(hInsertMenu, MF_POPUP, (UINT_PTR)hLinesMenu, L"Lines");
	AppendMenu(hInsertMenu, MF_POPUP, (UINT_PTR)hShapesMenu, L"Shapes");

	AppendMenu(hMapEditMenu, MF_POPUP, (UINT_PTR)hInsertMenu, L"Insert");

	//ADD RUN MENU FOR ALGORITHM
	AppendMenu(hMapEditMenu, MF_STRING, MAP_EDIT_RUN_ALGORITHM, L"Run");

	//ADD HELP MENU
	AppendMenu(hMapEditMenu, MF_STRING, MAP_EDIT_SCREEN_HELP, L"Help");

	SetMenu(hWnd, hMapEditMenu);
	MenuPtrs.push_back(&hMapEditMenu);

	//MAP AND BITMAP - SUBWINDOW TO HOLD BITMAP
	int Scale{ CalculateScale(BmpSideSize, MaxBitmapSize) };
	//to fix improper scaling of the window, can cause underscaling, but prevents cutoffs
	int ScalingFix{ static_cast<int>((0.8 * static_cast<double>(MaxBitmapSize)) / static_cast<double>(Scale)) };
	int BitmapWidth{ MaxBitmapSize + ScalingFix };
	int BitmapHeight{ MaxBitmapSize + ScalingFix };
	int BitmapXPos{ 0 };
	int BitmapYPos{ 0 };
	hBitmapHolder = CreateWindowW(L"Static", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER,
		BitmapXPos, BitmapYPos, BitmapWidth, BitmapHeight, hWnd, NULL, NULL, NULL);
	ChildWindowPtrs.push_back(&hBitmapHolder);

	//RESIZE PARENT WINDOW
	ParentHeight = BitmapHeight + 60;
	ParentWidth = BitmapWidth + 15;
	SetWindowPos(hWnd, NULL, NULL, NULL, ParentWidth, ParentHeight, SWP_NOMOVE);

	//SET THE USER MAP AS A SQUARE & FILL TO DESIRED SIZE
	UserMap = base_map(BmpSideSize, BmpSideSize);
	if (MapHeight > MapWidth) {
		//iterate over the right side of the map, filling out as walls
		for (int i{ 0 }; i < UserMap.get_rows(); i++) {
			for (int j{ MapWidth + 1 }; j < UserMap.get_cols(); j++) {
				UserMap.set_coord(i, j, wall);
			}
		}
	}
	else if (MapWidth > MapHeight) {
		//iterate over the bottom side of the map, filling out as walls
		for (int i{ MapHeight + 1 }; i < UserMap.get_rows(); i++) {
			for (int j{ 0 }; j < UserMap.get_cols(); j++) {
				UserMap.set_coord(i, j, wall);
			}
		}
	}
	MapIsShowing = true;
}

void DisplayUserMap(HDC hdc, const pixel_array& px_arr, const int scale) {
	if (scale < 1) {
		MessageBox(NULL, L"DisplayUserMap error: scale factor <1", L"Error", MB_ICONERROR);
		return;
	}

	//CREATE BITMAP OBJECT
	HDC memDC = CreateCompatibleDC(NULL);
	LONG bmpWidth = px_arr.get_cols()*scale;
	LONG bmpHeight = px_arr.get_rows()*scale;
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, bmpWidth, bmpHeight);

	//SELECT BITMAP INTO MEMORY FOR EDITING
	SelectObject(memDC, hBitmap);

	//ITERATE OVER BITMAP, SCALING AND SETTING PIXELS
	//px - keeps track of position in smaller px array
	int px_i{ 0 };
	//iterate over the bitmap, skipping the "added in" regions
	for (int bmp_i{ 0 }; bmp_i < bmpHeight; bmp_i += scale) {
		int px_j{ 0 };
		for (int bmp_j{ 0 }; bmp_j < bmpWidth; bmp_j += scale) {
			//iterate over the "added in" regions, setting the pixels to the corresponding points in px array
			for (int row{ 0 }; row < scale; row++) {
				for (int col{ 0 }; col < scale; col++) {
					SetPixel(memDC, bmp_j + col, bmp_i + row, px_arr(px_i, px_j));
				}
			}
			//move to the next point in the px array
			px_j += 1;
		}
		px_i += 1;
	}

	//DRAW MEMORY (BITMAP) CONTENTS TO SCREEN
	BitBlt(hdc, 0, 0, bmpWidth, bmpHeight, memDC, NULL, NULL, SRCCOPY);

	//FREE MEMORY SPACE
	DeleteObject(hBitmap);
	DeleteObject(memDC);
}