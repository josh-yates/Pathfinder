#include "filewindow.h"

//REGISTER FILE WINDOW CLASS WITH OS
void RegisterFileWindow(HINSTANCE hInst) {
	//same window class creation method as parent
	WNDCLASSW FileWindowClass{ 0 };
	FileWindowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;		//paint background colour
	FileWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);		//set cursor type
	FileWindowClass.hInstance = hInst;							//current instance of this window, provided by OS
	FileWindowClass.lpszClassName = L"FileWindowClass";			//window class identifier
	FileWindowClass.lpfnWndProc = FileWindowProcedure;		//provide a message handler

															//REGISTER WINDOW CLASS WITH OS
	if (!RegisterClassW(&FileWindowClass)) {
		MessageBox(NULL, L"Failed to register FileWindowClass", L"Error", MB_ICONWARNING);
	}
}

//DISPLAY FILE WINDOW ON SCREEN
void DisplayFileWindow(HWND hWnd, std::wstring FunctionType) {
	//CHECK FUNCTIONTYPE
	if (FunctionType != L"Open" && FunctionType != L"Save") {
		throw "DisplayFileWindow: Invalid function type";
	}

	//CREATE THE WINDOW
	hFileWindow = CreateWindowW(L"FileWindowClass", FunctionType.c_str(), WS_VISIBLE | WS_SYSMENU,
		200, 200, FileWidth, FileHeight, hWnd, NULL, NULL, NULL);

	//ADD INSTRUCTION MESSAGE
	int InstructionWidth{ 400 };
	int InstructionHeight{ 50 };
	int InstructionXPos{ (FileWidth - InstructionWidth) / 2 };
	int InstructionYPos{ 10 };
	std::wstringstream msg_stream;
	msg_stream << L"Enter name to " << FunctionType << " the file. Specify path if not current directory.";
	hFileInstructions = CreateWindowW(L"Static", msg_stream.str().c_str(), WS_VISIBLE | WS_CHILD | SS_CENTER,
		InstructionXPos, InstructionYPos, InstructionWidth, InstructionHeight, hFileWindow, NULL, NULL, NULL);

	//ADD FILE PATH INPUT
	int PathInputWidth{ 300 };
	int PathInputHeight{ 20 };
	int PathInputXPos{ (FileWidth - PathInputWidth) / 2 };
	int PathInputYPos{ InstructionYPos + InstructionHeight + 10 };
	hFilePathInput = CreateWindow(L"Edit", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
		PathInputXPos, PathInputYPos, PathInputWidth, PathInputHeight, hFileWindow, NULL, NULL, NULL);

	//ADD OPEN/SAVE AND CANCEL BUTTONS
	int ButtonFunction;
	if (FunctionType == L"Open") {
		ButtonFunction = FILE_WINDOW_OPEN;
	}
	else {
		ButtonFunction = FILE_WINDOW_SAVE;
	}
	int CancelButtonWidth{ 100 };
	int CancelButtonHeight{ 30 };
	int FunctionButtonWidth{ 100 };
	int FunctionButtonHeight{ 30 };
	int CancelButtonXPos{ (FileWidth / 2) - CancelButtonWidth - 10 };
	int CancelButtonYPos{ PathInputYPos + PathInputHeight + 10 };
	int FunctionButtonXPos{ (FileWidth / 2) + 10 };
	int FunctionButtonYPos{ CancelButtonYPos };
	hFileCancelButton = CreateWindowW(L"Button", L"Cancel", WS_VISIBLE | WS_CHILD | SS_CENTER,
		CancelButtonXPos, CancelButtonYPos, CancelButtonWidth, CancelButtonHeight, hFileWindow, (HMENU)FILE_WINDOW_CANCEL, NULL, NULL);
	hFileFunctionButton = CreateWindowW(L"Button", FunctionType.c_str(), WS_VISIBLE | WS_CHILD | SS_CENTER,
		FunctionButtonXPos, FunctionButtonYPos, FunctionButtonWidth, FunctionButtonHeight, hFileWindow, (HMENU)ButtonFunction, NULL, NULL);
}
