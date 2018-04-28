#include "parentwindow.h"

//REGISTER PARENT WINDOW CLASS WITH OS
void RegisterParentWindow(HINSTANCE hInst) {
	//CREATE WINDOW CLASS
	WNDCLASSW ParentWindowClass{ 0 };
	ParentWindowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;		//paint background colour
	ParentWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);	//set cursor type
	ParentWindowClass.hInstance = hInst;						//current instance of this window, provided by OS
	ParentWindowClass.lpszClassName = L"ParentWindowClass";		//window class identifier
	ParentWindowClass.lpfnWndProc = ParentWindowProcedure;		//provide a message handler

																//REGISTER WINDOW CLASS WITH OS
	if (!RegisterClassW(&ParentWindowClass)) {
		//throw error if unable to register class
		MessageBox(NULL, L"Failed to register ParentWindowClass", L"Error", MB_ICONWARNING);
	}
}

//DISPLAY PARENT WINDOW ON SCREEN
void DisplayParentWindow() {
	//CREATE WINDOW OBJECT
	hParentWindow = CreateWindowW(
		L"ParentWindowClass",		//the window class to make this object part of
		L"Pathfinder",				//window title
		WS_SYSMENU | WS_VISIBLE,	//window display properties (non-resizeable, visible)
		100, 100,					//x,y position of top LH corner on screen
		ParentWidth, ParentHeight,					//x,y dimensions of window
		NULL, NULL, NULL, NULL);
}

//GENERAL/PROCEDURE FUNCTIONS
void DeleteParentWindowContents(HWND hWnd) {
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

void ShowHelpDialog(HWND hWnd) {
	//create help/instruction message
	std::wstringstream HelpStream;
	HelpStream << L"PATHFINDER HELP PAGE" << std::endl << std::endl;
	HelpStream << L"Pathfinder.exe is a program for creating grid-based maps and finding routes through them." << std::endl << std::endl;
	HelpStream << L"START SCREEN:" << std::endl;
	HelpStream << L"You can load a map from a file for editing, or create a new map on the new map screen. "
		<< L"If the loaded map is already solved, the route will be cleared." << std::endl << std::endl;
	HelpStream << L"CREATE NEW MAP SCREEN:" << std::endl;
	HelpStream << L"You can enter the width and height values for the map, between " << MinMapSize << L" and " << MaxMapSize << L" tiles."
		<< L" Walls are automatically added outside the map space. If the width and height are not equal, the map edge size is the bigger of the two"
		<< L" and walls are added to reach the desired dimensions." << std::endl << std::endl;
	HelpStream << L"MAP EDIT SCREEN:" << std::endl;
	HelpStream << L"Here you can add obstacles and the start and end points to create your map. The controls for all obstacles (point, line shape) are toggleable, "
		<< L"so you can use the same obstacle multiple times without needing to revisit the menu. Switching to another obstacle will toggle-off the previous one. "
		<< L" Free spaces are in the points menu, and can be used to remove obstacle/start/end points, with the exception of walls. "
		<< L"Free spaces are also toggleable. The start and end points are not toggleable and can only be placed once, unless the original is removed with a free space. "
		<< L"The path can be found by clicking Run, but the algorithm will only run when both the start and end points are present. "
		<< L"If no route can be found a notification appears. The algorithm is quick, but there may be a noticeable delay on large maps "
		<< L"or maps with a lot of free space. The map is not editable after "
		<< L"the algorithm has been run. If the map is saved after the algorithm is run, it will contain details of the route found.";
	MessageBox(hWnd, HelpStream.str().c_str(), L"Help page", NULL);
}

void DisplayStartScreen(HWND hWnd) {
	//CLEAR CHILDREN WINDOWS SET PARENT WINDOW DIMENSIONS AND RESIZE
	DeleteParentWindowContents(hWnd);
	ParentHeight = 250;
	ParentWidth = 300;
	SetWindowPos(hWnd, NULL, NULL, NULL, ParentWidth, ParentHeight, SWP_NOMOVE);

	//START MESSAGE
	int StartMessageWidth{ 180 };
	int StartMessageHeight{ 40 };
	int StartMessageXPos{ (ParentWidth - StartMessageWidth) / 2 };
	int StartMessageYpos{ 10 };
	hStartMessage = CreateWindowW(L"Static", L"Welcome to Pathfinder.exe\nChoose an option:", WS_VISIBLE | WS_CHILD | SS_CENTER,
		StartMessageXPos, StartMessageYpos, StartMessageWidth, StartMessageHeight, hWnd, NULL, NULL, NULL);

	//GENERAL BUTTON DIMENSIONS
	int ButtonWidth{ 110 };
	int ButtonHeight{ 30 };

	//OPEN FILE BUTTON
	int OpenButtonWidth{ ButtonWidth };
	int OpenButtonHeight{ ButtonHeight };
	int OpenButtonXPos{ (ParentWidth - OpenButtonWidth) / 2 };
	int OpenButtonYPos{ StartMessageYpos + StartMessageHeight + 10 };
	hOpenButton = CreateWindowW(L"Button", L"Open from file", WS_VISIBLE | WS_CHILD | SS_CENTER,
		OpenButtonXPos, OpenButtonYPos, OpenButtonWidth, OpenButtonHeight, hWnd, (HMENU)START_SCREEN__OPEN, NULL, NULL);

	//NEW MAP BUTTON
	int NewButtonWidth{ ButtonWidth };
	int NewButtonHeight{ ButtonHeight };
	int NewButtonXPos{ (ParentWidth - NewButtonWidth) / 2 };
	int NewButtonYPos{ OpenButtonYPos + OpenButtonHeight + 10 };
	hNewButton = CreateWindowW(L"Button", L"Create new map", WS_VISIBLE | WS_CHILD | SS_CENTER,
		NewButtonXPos, NewButtonYPos, NewButtonWidth, NewButtonHeight, hWnd, (HMENU)START_SCREEN__NEW, NULL, NULL);

	//HELP BUTTON
	int HelpButtonWidth{ ButtonWidth };
	int HelpButtonHeight{ ButtonHeight };
	int HelpButtonXPos{ (ParentWidth - HelpButtonWidth) / 2 };
	int HelpButtonYPos{ NewButtonYPos + NewButtonHeight + 10 };
	hHelpButton = CreateWindowW(L"Button", L"Instructions", WS_VISIBLE | WS_CHILD | SS_CENTER,
		HelpButtonXPos, HelpButtonYPos, HelpButtonWidth, HelpButtonHeight, hWnd, (HMENU)SHOW_HELP_PAGE, NULL, NULL);

	//TODO add messages for the two buttons
	ChildWindowPtrs.push_back(&hStartMessage);
	ChildWindowPtrs.push_back(&hOpenButton);
	ChildWindowPtrs.push_back(&hNewButton);
	ChildWindowPtrs.push_back(&hHelpButton);
}

void DisplayNewMapScreen(HWND hWnd) {
	//CLEAR THE WINDOW AND RESIZE
	DeleteParentWindowContents(hWnd);
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
	hNewMapWidthInput = CreateWindowW(L"Edit", L"10", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOVSCROLL | SS_CENTER,
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

void DisplayMapEditScreen(HWND hWnd, const int MapHeight, const int MapWidth, const bool OpenedFromFile) {
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
	DeleteParentWindowContents(hWnd);

	//ADD MAIN MENU
	hMapEditMenu = CreateMenu();

	//ADD FILE MENU
	//TODO add messages for menu buttons
	HMENU hFileMenu{ CreateMenu() };
	AppendMenu(hFileMenu, MF_STRING, MAP_EDIT_SCREEN_RESTART, L"Restart");
	AppendMenu(hFileMenu, MF_STRING, MAP_EDIT_SCREEN_SAVE, L"Save Map");
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
	AppendMenu(hMapEditMenu, MF_STRING, SHOW_HELP_PAGE, L"Help");

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

	//IF MAP WAS NOT OPENED FROM A FILE
	if (!OpenedFromFile) {
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
	}
	MapIsShowing = true;
}

void DisplayUserMap(HDC hdc, const base_map& map_in, const int scale) {
	if (scale < 1) {
		MessageBox(NULL, L"DisplayUserMap error: scale factor <1", L"Error", MB_ICONERROR);
		return;
	}

	pixel_array px_arr(map_in, int_to_px_translate);

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
					//add grid lines
					if ((col == scale - 1 || row == scale - 1)
						&& map_in(px_i, px_j) != wall && map_in(px_i, px_j) != obstacle) {
						SetPixel(memDC, bmp_j + col, bmp_i + row, color_grey);
					}
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

void CreateNewMap(HWND hWnd) {
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
		DisplayMapEditScreen(hWnd, height_out, width_out, false);
	}
}

void AddStartPoint() {
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
}

void AddEndPoint() {
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
}

void AddObstaclePoint() {
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
}

void AddFreeSpacePoint() {
	if (AlgorithmRun) {
		MessageBox(hBitmapHolder, L"Algorithm already run", L"Algorithm result", MB_ICONINFORMATION);
		MapEditAddingFreeSpace = false;
	}
	//toggle the free point adding
	else if (MapEditAddingFreeSpace) {
		FalseAllMapEdits();
	}
	else if (!MapEditAddingFreeSpace) {
		FalseAllMapEdits();
		MapEditAddingFreeSpace = true;
	}
}

void AddLShape() {
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
}

void AddSmallSquare() {
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
}

void AddBigSquare() {
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
}

void AddHLine() {
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
}

void AddVLine() {
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
}

void RunAlgorithm() {
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
			MessageBox(hBitmapHolder, L"No path found", L"Algorithm result", MB_ICONINFORMATION);
		}
		for (auto coord = path_coords.begin(); coord != path_coords.end(); coord++) {
			if (UserMap(coord->at(0), coord->at(1)) != start_point && UserMap(coord->at(0), coord->at(1)) != end_point) {
				UserMap.set_coord(coord->at(0), coord->at(1), path);
			}
		}
		AlgorithmRun = true;
	}
}

void MouseClickPlaceShape(const int MapXClick, const int MapYClick) {
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