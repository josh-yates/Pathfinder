//MAIN PROGRAM FILE FOR PATHFINDER PROJECT

#define NOMINMAX		//handles the clash between windows.h min/max and limits min/max

//INCLUDES
#include <Windows.h>
#include <vector>
#include <sstream>
#include <limits>
#include <cmath>

//TODO maybe have parent window size as global variable, could negate ParentWidth etc

//CUSTOM MESSAGE DEFINITIONS
#define START_SCREEN__OPEN 1
#define START_SCREEN__NEW 2
#define NEW_MAP_SCREEN_CREATE 3

//PARENT WINDOW VARIABLES
int ParentHeight{ 200 };
int ParentWidth{ 300 };
std::vector<HWND*> ChildWindows;


//FORWARD DECLARATIONS
LRESULT CALLBACK window_procedure(HWND, UINT, WPARAM, LPARAM);		//message handler
void GetWindowSize(HWND, int&, int&);
void DeleteChildren();
bool CheckTextPosInt(const wchar_t*, const int);					//for checking text input is an integer

void DisplayStartScreen(HWND);										//start screen, with a message and two buttons
HWND hStartMessage;
HWND hOpenButton;
HWND hNewButton;

void DisplayNewMapScreen(HWND);										//screen shown when new map is to be created
HWND hNewMapInstructions;
HWND hNewMapHeightLabel;
HWND hNewMapHeightInput;
HWND hNewMapWidthLabel;
HWND hNewMapWidthInput;
HWND hNewMapCreateButton;

void DisplayMapEditScreen(HWND, int, int);									//screen shown when map is being edited
HBITMAP hMapBmp;
HMENU hMapEditMenu;


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
		case NEW_MAP_SCREEN_CREATE:
			//check height and width inputs
			wchar_t height_in[100];
			wchar_t width_in[100];
			GetWindowTextW(hNewMapHeightInput, height_in, 100);
			GetWindowTextW(hNewMapWidthInput, width_in, 100);
			if (CheckTextPosInt(height_in, 4) && CheckTextPosInt(width_in, 4)) {
				MessageBox(hWnd, L"Inputs valid", L"Yes!", MB_ICONQUESTION);
			}
			else {
				MessageBox(hWnd, L"Inputs not valid", L"No!", MB_ICONERROR);
			}
			break;
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

void DeleteChildren() {
	//iterate over child windows, destroying them all
	for (auto child = ChildWindows.begin(); child != ChildWindows.end(); child++) {
		DestroyWindow(**child);
	}
	//clear the vector
	ChildWindows.clear();
}

bool CheckTextPosInt(const wchar_t* input_wchar,const int min_val) {
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
		return false;
	}
	//else, if anything followed the number, fail
	else if ((buff_peek = static_cast<wchar_t>(input_stream.peek())) != L'\n') {
		input_stream.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
		return false;
	}
	//else, it is a number, now check it is >= min_val & is an integer
	else if (input_double < min_val) {
		//use stream to output to debug
		std::ostringstream debugstream;
		debugstream << "Input value: " << input_double << std::endl;
		OutputDebugStringA(debugstream.str().c_str());
		return false;
	}
	else {
		double int_part, fract_part;
		fract_part = modf(input_double, &int_part);
		if (fract_part == 0) {
			//it is an integer
			return true;
		}
		else {
			return false;
		}
	}
}

//SCREEN FUNCTIONS

void DisplayStartScreen(HWND hWnd) {
	//CLEAR CHILDREN WINDOWS SET PARENT WINDOW DIMENSIONS AND RESIZE
	DeleteChildren();
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
	ChildWindows.push_back(&hStartMessage);
	ChildWindows.push_back(&hOpenButton);
	ChildWindows.push_back(&hNewButton);
}

void DisplayNewMapScreen(HWND hWnd) {
	//CLEAR THE WINDOW AND RESIZE
	DeleteChildren();
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

	ChildWindows.push_back(&hNewMapInstructions);
	ChildWindows.push_back(&hNewMapWidthLabel);
	ChildWindows.push_back(&hNewMapWidthInput);
	ChildWindows.push_back(&hNewMapHeightLabel);
	ChildWindows.push_back(&hNewMapHeightInput);
	ChildWindows.push_back(&hNewMapCreateButton);
}