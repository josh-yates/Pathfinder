#ifndef FILEWINDOW_INCLUDED
#define FILEWINDOW_INCLUDED

#include <Windows.h>
#include <stdexcept>
#include "program_definitions.h"

//REGISTER FILE WINDOW CLASS WITH OS
void RegisterFileWindow(HINSTANCE);

//DISPLAY FILE WINDOW ON SCREEN
void DisplayFileWindow(HWND hWnd, std::wstring FunctionType);

//FILE WINDOW PROCEDURE - defined in WinMain
LRESULT CALLBACK FileWindowProcedure(HWND hWnd, UINT message, WPARAM wp, LPARAM lp);
#endif