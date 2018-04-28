#ifndef PROGRAM_DEFINITIONS_INCLUDED
#define PROGRAM_DEFINITIONS_INCLUDED

#define NOMINMAX	//handles the clash between windows.h min/max and limits min/max

//VARIABLES AND FUNCTIONS USED BY THE MAIN PROGRAM

#include <Windows.h>
#include <vector>
#include <fstream>
#include "base_map.h"


//CUSTOM MESSAGE DEFINITIONS
#define START_SCREEN__OPEN 1
#define START_SCREEN__NEW 2
#define NEW_MAP_SCREEN_CREATE 3
#define MAP_EDIT_SCREEN_EXIT 4
#define MAP_EDIT_SCREEN_RESTART 5
#define MAP_EDIT_SCREEN_ADD_POINTS_OBSTACLE 6
#define MAP_EDIT_SCREEN_ADD_POINTS_START 7
#define MAP_EDIT_SCREEN_ADD_POINTS_END 8
#define MAP_EDIT_RUN_ALGORITHM 9
#define MAP_EDIT_SCREEN_ADD_L_SHAPE 10
#define MAP_EDIT_SCREEN_ADD_SMALL_SQUARE 11
#define MAP_EDIT_SCREEN_ADD_FREE_SPACE 12
#define MAP_EDIT_SCREEN_ADD_BIG_SQUARE 13
#define	MAP_EDIT_SCREEN_ADD_V_LINE 14
#define MAP_EDIT_SCREEN_ADD_H_LINE 15
#define SHOW_HELP_PAGE 16
#define FILE_WINDOW_OPEN 17
#define FILE_WINDOW_SAVE 18
#define FILE_WINDOW_CANCEL 19
#define MAP_EDIT_SCREEN_SAVE 20
#define MAP_OPENED_FROM_FILE 21

//-----VARIABLES-----

//MAP RELATED VARIABLES
extern const int MinMapSize;		//ranges for size of UserMap
extern const int MaxMapSize;
extern const int MaxBitmapSize;		//size of bitmap displayed
extern base_map UserMap;			//base_map to be worked with

//WINDOW RELATED VARIABLES
extern int ParentHeight;			//sizes of the main/parent window
extern int ParentWidth;
extern int FileHeight;				//sizes of file dialog window
extern int FileWidth;
extern bool MapIsShowing;			//if the map is on the screen
extern bool AlgorithmRun;			//whether the algorithm has been run

//MAIN/DIALOG WINDOW HANDLES
extern HWND hParentWindow;					//handle to the main/parent window
extern HWND hFileWindow;					//handle to dialog box for file save/open
extern std::vector<HWND*> ChildWindowPtrs;	//container for all the children of the main/parent window in use
extern std::vector<HMENU*> MenuPtrs;		//container for all the menus of the main/parent window in use

//GROUPED SCREEN VARIABLES/HANDLES

//FILE DIALOG WINDOW GROUP			//these are shown in hFileWindow, the others are in hParentWindow
extern HWND hFileInstructions;		//static text box with open/save instructions
extern HWND hFilePathInput;			//input box for file path/name
extern HWND hFileCancelButton;		//buttons for cancelling or open/save
extern HWND hFileFunctionButton;

//MAP EDIT SCREEN GROUP
extern HMENU hMapEditMenu;				//menu for the map editing tools
extern HWND hBitmapHolder;				//child window of main/parent window for holding the bitmap
extern bool MapEditAddingFreeSpace;		//trackers for toggling map edit tools
extern bool MapEditAddingObstacle;
extern bool MapEditAddingStart;
extern bool MapEditAddingEnd;
extern bool MapEditAddingL_Shape;
extern bool MapEditAddingSmallSquare;
extern bool MapEditAddingBigSquare;
extern bool MapEditAddingVLine;
extern bool MapEditAddingHLine;

//START SCREEN GROUP
extern HWND hStartMessage;		//static text box with weclome message
extern HWND hOpenButton;		//option buttons
extern HWND hNewButton;
extern HWND hHelpButton;

//NEW MAP SCREEN GROUP
extern HWND hNewMapInstructions;	//text boxes, buttons and input fields for entering map dimensions
extern HWND hNewMapHeightLabel;
extern HWND hNewMapHeightInput;
extern HWND hNewMapWidthLabel;
extern HWND hNewMapWidthInput;
extern HWND hNewMapCreateButton;

//-----FUNCTIONS-----
void FalseAllMapEdits();														//disables all the edit trackers
bool CheckTextPosInt(HWND, const std::wstring, const int, const int, int&);		//check text input is a positive integer in a range
int CalculateScale(const int, const int);										//calculate scale up factor for map->bitmap
bool SaveMapToFile(HWND, base_map&, std::wstring);								//save map to text file
bool OpenMapFromFile(HWND, base_map&, std::wstring);							//read map from text file, with checks

#endif