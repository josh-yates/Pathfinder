//MAIN PROGRAM FILE FOR PATHFINDER PROJECT


//INCLUDES
#include <Windows.h>

#define CLOSE_OPEN_BUTTON 1

//FORWARD DECLARATIONS
LRESULT CALLBACK window_procedure(HWND, UINT, WPARAM, LPARAM);		//message handler

void DisplayStartScreen(HWND);										//start screen, with a message and two buttons
HWND hStartMessage;
HWND hOpenButton;
HWND hNewButton;
void DisplayNewMapScreen(HWND);										//screen shown when new map is to be created

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
		300, 200,					//x,y dimensions of window
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
		}
		break;
	//WINDOW CLOSURE
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
	//DEFAULT PROCEDURE
	default:
		return DefWindowProcW(hWnd, message, wp, lp);
		break;
	}
}

void DisplayStartScreen(HWND hWnd) {
	//GET PARENT WINDOW DIMENSIONS
	RECT ParentRect;
	int ParentWidth, ParentHeight;
	if (!GetWindowRect(hWnd, &ParentRect)) {
		//break and tell user
		MessageBox(NULL, L"Unable to access parent window size", L"Error", MB_ICONERROR);
		return;
	}
	ParentWidth = ParentRect.right - ParentRect.left;
	ParentHeight = ParentRect.bottom - ParentRect.top;

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
		OpenButtonXPos, OpenButtonYPos, OpenButtonWidth, OpenButtonHeight, hWnd, (HMENU)CLOSE_OPEN_BUTTON, NULL, NULL);

	//NEW MAP BUTTON
	int NewButtonWidth{ 110 };
	int NewButtonHeight{ 30 };
	int NewButtonXPos{ (ParentWidth - NewButtonWidth) / 2 };
	int NewButtonYPos{ OpenButtonYPos + NewButtonHeight + 10 };
	hNewButton = CreateWindowW(L"Button", L"Create new map", WS_VISIBLE | WS_CHILD | SS_CENTER,
		NewButtonXPos, NewButtonYPos, NewButtonWidth, NewButtonHeight, hWnd, NULL, NULL, NULL);

	//TODO add messages for the two buttons
}