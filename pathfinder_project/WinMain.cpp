//MAIN PROGRAM FILE FOR PATHFINDER PROJECT

/*Pathfinder is an interface-based program for finding the shortest route through a user created map.
  Users can add start and end points and a series of obstacle shapes to build their map. The a-star 
  algorithm is used to find the shortest route. Maps can be saved to text files and re-opened for
  editing and re-solving.
*/

/*IMPORTANT: PROJECT MUST BE BUILT AS A WIN32 PROJECT, NOT A CONSOLE APPLICATION*/

//INCLUDES
#include <Windows.h>
#include <string>
#include <stdexcept>
#include "parentwindow.h"
#include "filewindow.h"

//MAIN PROGRAM (API EQUIVALENT OF INT MAIN)
int CALLBACK WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
	try {
		//REGISTER WINDOW CLASSES
		RegisterParentWindow(hInst);			//register window classes with OS and show the main/parent window
		RegisterFileWindow(hInst);
		
		DisplayParentWindow();

		//CREATE MESSAGE LOOP
		MSG message{ 0 };						//object to store messages
		while (GetMessage(&message, NULL, NULL, NULL)) {
			TranslateMessage(&message);
			DispatchMessage(&message);			//translate message and send to related window procedure
		}
	}
	//catch errors and put in message box
	catch (std::invalid_argument& inval_arg) {
		//copy string to wstring
		std::string temp_string(inval_arg.what());
		std::wstring errmsg(temp_string.length(), L' ');
		std::copy(temp_string.begin(), temp_string.end(), errmsg.begin());
		MessageBox(hParentWindow, errmsg.c_str(), L"Programming error", MB_ICONERROR);
	}
	catch (...) {
		MessageBox(hParentWindow, L"Unknown error", L"Programming error", MB_ICONERROR);
	}
	return 0;
}

/*WINDOW PROCEDURES
LRESULT - pointer to long int
CALLBACK - __stdcall, just specifies that data is passed right to left*/
LRESULT CALLBACK ParentWindowProcedure(HWND hWnd, UINT message, WPARAM wp, LPARAM lp) {
	//case dependent message handler - message stored in message
	switch (message) {
	//WINDOW CREATION
	case WM_CREATE:
		DisplayStartScreen(hWnd);
		break;
	//COMMANDS FROM ITEMS IN WINDOW (buttons, menus etc) - stored in wp
	case WM_COMMAND:
		switch (wp) {

		//HELP MESSAGE
		case SHOW_HELP_PAGE:
			ShowHelpDialog(hWnd);
			break;

		//START SCREEN MESSAGES
		case START_SCREEN__OPEN:
			DisplayFileWindow(hWnd,L"Open");
			break;
		case START_SCREEN__NEW:
			DisplayNewMapScreen(hWnd);
			break;
		case NEW_MAP_SCREEN_CREATE:
			CreateNewMap(hWnd);
			break; 

		//MAP EDIT SCREEN FILE (ETC) MESSAGES

		case MAP_EDIT_SCREEN_EXIT:
			//call the wm_destroy message to close the window
			SendMessage(hWnd, WM_DESTROY, NULL, NULL);
			break;
		case MAP_EDIT_SCREEN_SAVE:
			DisplayFileWindow(hWnd, L"Save");
			break;
		case MAP_EDIT_SCREEN_RESTART:
			DisplayStartScreen(hWnd);
			break;

		//INSERT POINTS MESSAGES

		case MAP_EDIT_SCREEN_ADD_POINTS_START:
			AddStartPoint();
			break;
		case MAP_EDIT_SCREEN_ADD_POINTS_END:
			AddEndPoint();
			break;
		case MAP_EDIT_SCREEN_ADD_POINTS_OBSTACLE:
			AddObstaclePoint();
			break;
		case MAP_EDIT_SCREEN_ADD_FREE_SPACE:
			AddFreeSpacePoint();
			break;

		//INSERT SHAPE MESSAGES

		case MAP_EDIT_SCREEN_ADD_L_SHAPE:
			AddLShape();
			break;
		case MAP_EDIT_SCREEN_ADD_SMALL_SQUARE:
			AddSmallSquare();
			break;
		case MAP_EDIT_SCREEN_ADD_BIG_SQUARE:
			AddBigSquare();
			break;
		//INSERT LINES MESSAGES
		case MAP_EDIT_SCREEN_ADD_H_LINE:
			AddHLine();
			break;
		case MAP_EDIT_SCREEN_ADD_V_LINE:
			AddVLine();
			break;

		//ALGORITHM MESSAGE

		case MAP_EDIT_RUN_ALGORITHM:
			RunAlgorithm();
			//force repaint of window
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	//WINDOW CLOSURE
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
	//BITMAP PAINTING
	case WM_PAINT: {
		PAINTSTRUCT ps;		//painting tools
		HDC hdc;
		if (MapIsShowing) {
			//paint the bitmap in the bitmap holder window
			hdc = BeginPaint(hBitmapHolder, &ps);
			int Scale{ CalculateScale(UserMap.get_cols(),MaxBitmapSize) };
			DisplayUserMap(hdc, UserMap, Scale);
			EndPaint(hBitmapHolder, &ps);
			//paint the main window
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		else {
			//paint the main window
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break; 
	}
	//LEFT MOUSE CLICK
	case WM_LBUTTONDOWN: {
		if (MapIsShowing && !AlgorithmRun) {
			//store mouse position in POINT structure
			POINT MousePos;
			MousePos.x = LOWORD(lp);
			MousePos.y = HIWORD(lp);
			//calculate scale used to get to bitmap size and scale coordinates down
			int Scale{ CalculateScale(UserMap.get_cols(),MaxBitmapSize) };
			int MapXClick{ static_cast<int>(static_cast<double>(MousePos.x) / static_cast<double>(Scale)) };
			int MapYClick{ static_cast<int>(static_cast<double>(MousePos.y) / static_cast<double>(Scale)) };
			MouseClickPlaceShape(MapXClick, MapYClick);
		}
		//force repaint
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	//DEFAULT PROCEDURE HANDLES ALL OTHER MESSAGES
	default:
		return DefWindowProcW(hWnd, message, wp, lp);
		break;
	}
	return 0;
}

LRESULT CALLBACK FileWindowProcedure(HWND hWnd, UINT message, WPARAM wp, LPARAM lp) {
	//case dependent message handler
	switch (message) {
	//WINDOW CREATION
	case WM_CREATE:
		//disable clicking on the parent window, so only the file window can be used
		EnableWindow(hParentWindow, false);
		break;
	//WINDOW CLOSURE
	case WM_CLOSE:
		//re-enable the parent window and close the file window
		EnableWindow(hParentWindow, true);
		DestroyWindow(hWnd);
		BringWindowToTop(hParentWindow);
		break;
	//CUSTOM MESSAGES FROM INTERFACE
	case WM_COMMAND:
		switch (wp) {
		//WINDOW CLOSURE
		case FILE_WINDOW_CANCEL:
			//close the file window
			SendMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;
		//OPEN FILE BUTTON
		case FILE_WINDOW_OPEN: {
			//get input from the file input box and store in path_in
			wchar_t path_in[MAX_PATH];
			GetWindowTextW(hFilePathInput, path_in, MAX_PATH);
			//try opening the map and closing the file window, else there was an error so clear the input
			if (OpenMapFromFile(hWnd, UserMap, path_in)) {
				DisplayMapEditScreen(hParentWindow, UserMap.get_rows(), UserMap.get_cols(), true);
				SendMessage(hWnd, WM_CLOSE, NULL, NULL);
			}
			else {
				SetWindowTextW(hFilePathInput, L"");
			}
			break;
		}
		//SAVE FILE BUTTON
		case FILE_WINDOW_SAVE: {
			//get input from the file input box and store in path_in
			wchar_t path_in[MAX_PATH];
			GetWindowTextW(hFilePathInput, path_in, MAX_PATH);
			//try saving the map and closing the file window, else there was an error so clear the input
			if (SaveMapToFile(hWnd, UserMap, path_in)) {
				SendMessage(hWnd, WM_CLOSE, NULL, NULL);
			}
			else {
				SetWindowTextW(hFilePathInput, L"");
			}
			break;
		}
		}
		break;
	//DEFAULT PROCEDURE
	default:
		return DefWindowProcW(hWnd, message, wp, lp);
		break;
	}
	return 0;
}