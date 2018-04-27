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