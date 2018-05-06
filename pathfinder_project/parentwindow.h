//declaration of the parent window related functions

#ifndef PARENTWINDOW_INCLUDED
#define PARENTWINDOW_INCLUDED

#include <Windows.h>
#include "pixel_array.h"
#include "astar.h"
#include "derived_shapes.h"
#include "program_definitions.h"

//REGISTER PARENT WINDOW CLASS WITH OS
void RegisterParentWindow(HINSTANCE);

//DISPLAY PARENT WINDOW ON SCREEN
void DisplayParentWindow();

//PARENT WINDOW PROCEDURE
LRESULT CALLBACK ParentWindowProcedure(HWND, UINT, WPARAM, LPARAM);

//GENERAL/PROCEDURE FUNCTIONS
void DeleteParentWindowContents(HWND);								//destroys all the parent window's children in use
void ShowHelpDialog(HWND);											//show help/instruction window
void DisplayStartScreen(HWND);										//show specified screens
void DisplayNewMapScreen(HWND);
void DisplayMapEditScreen(HWND, const int, const int, const bool);
void DisplayUserMap(HDC, const base_map&, const int);
void CreateNewMap(HWND);
void AddStartPoint();
void AddEndPoint();
void AddObstaclePoint();
void AddFreeSpacePoint();
void AddLShape();
void AddSmallSquare();
void AddBigSquare();
void AddHLine();
void AddVLine();
void RunAlgorithm();
void MouseClickPlaceShape(const int, const int);
#endif