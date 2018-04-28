//MAIN PROGRAM FILE FOR PATHFINDER PROJECT

//INCLUDES
#include <Windows.h>
#include "parentwindow.h"
#include "filewindow.h"

//MAIN PROGRAM
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {

	//register window classes with OS
	RegisterParentWindow(hInst);
	RegisterFileWindow(hInst);

	DisplayParentWindow();

	//CREATE MESSAGE LOOP
	MSG message{ 0 };						//object to store messages
	while (GetMessage(&message, NULL, NULL, NULL)) {
		TranslateMessage(&message);
		DispatchMessage(&message);			//translate message and send to related window procedure
	}

	return 0;
}

//WINDOW PROCEDURES
LRESULT CALLBACK ParentWindowProcedure(HWND hWnd, UINT message, WPARAM wp, LPARAM lp) {
	//case dependent message handler
	switch (message) {
	//WINDOW CREATION
	case WM_CREATE:
		DisplayStartScreen(hWnd);
		break;
		return 0;
	//COMMANDS FROM ITEMS IN WINDOW
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

		//MAP EDIT FILE (ETC) MESSAGES

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

		//POINTS MESSAGES

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

		//SHAPE MESSAGES

		case MAP_EDIT_SCREEN_ADD_L_SHAPE:
			AddLShape();
			break;
		case MAP_EDIT_SCREEN_ADD_SMALL_SQUARE:
			AddSmallSquare();
			break;
		case MAP_EDIT_SCREEN_ADD_BIG_SQUARE:
			AddBigSquare();
			break;
		//LINES MESSAGES
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
		return 0;
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
			int Scale{ CalculateScale(UserMap.get_cols(),MaxBitmapSize) };
			DisplayUserMap(hdc, UserMap, Scale);
			EndPaint(hBitmapHolder, &ps);
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		else {
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break; 
		return 0;
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
			MouseClickPlaceShape(MapXClick, MapYClick);
		}
		//force repaint
		InvalidateRect(hWnd, NULL, FALSE);
		break;
		return 0;
	}
	//DEFAULT PROCEDURE
	default:
		return DefWindowProcW(hWnd, message, wp, lp);
		break;
	}
	return 0;
}

LRESULT CALLBACK FileWindowProcedure(HWND hWnd, UINT message, WPARAM wp, LPARAM lp) {
	switch (message) {
	case WM_CREATE:
		EnableWindow(hParentWindow, false);
		break;
		return 0;
	case WM_CLOSE:
		EnableWindow(hParentWindow, true);
		DestroyWindow(hWnd);
		BringWindowToTop(hParentWindow);
		break;
		return 0;
	case WM_COMMAND:
		switch (wp) {
		case FILE_WINDOW_CANCEL:
			SendMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;
		case FILE_WINDOW_OPEN: {
			//get input
			wchar_t path_in[MAX_PATH];
			GetWindowTextW(hFilePathInput, path_in, MAX_PATH);
			if (OpenMapFromFile(hWnd, UserMap, path_in)) {
				DisplayMapEditScreen(hParentWindow, UserMap.get_rows(), UserMap.get_cols(), true);
				SendMessage(hWnd, WM_CLOSE, NULL, NULL);
			}
			else {
				SetWindowTextW(hFilePathInput, L"");
			}
			break;
		}
		case FILE_WINDOW_SAVE: {
			//get input
			wchar_t path_in[MAX_PATH];
			GetWindowTextW(hFilePathInput, path_in, MAX_PATH);
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
		return 0;
	default:
		return DefWindowProcW(hWnd, message, wp, lp);
		break;
	}
	return 0;
}