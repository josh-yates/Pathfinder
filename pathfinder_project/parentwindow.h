//declaration of the parent window functions

#ifndef PARENTWINDOW_INCLUDED
#define PARENTWINDOW_INCLUDED

#include <Windows.h>
#include "program_definitions.h"

//REGISTER PARENT WINDOW CLASS WITH OS
void RegisterParentWindow(HINSTANCE);

//PARENT WINDOW PROCEDURE
LRESULT CALLBACK ParentWindowProcedure(HWND, UINT, WPARAM, LPARAM);

#endif