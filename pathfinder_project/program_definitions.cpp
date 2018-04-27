#include "program_definitions.h"

//MAP RELATED VARIABLES
const int MinMapSize{ 2 };
const int MaxMapSize{ 50 };
const int MaxBitmapSize{ 500 };
base_map UserMap(4, 4);

//WINDOW RELATED VARIABLES
int ParentHeight{ 200 };
int ParentWidth{ 300 };
int FileHeight{ 200 };
int FileWidth{ 400 };
bool MapIsShowing{ false };
bool AlgorithmRun{ false };

//MAIN/DIALOG WINDOW HANDLES
HWND hParentWindow;
HWND hFileWindow;
std::vector<HWND*> ChildWindowPtrs;
std::vector<HMENU*> MenuPtrs;

//GROUPED SCREEN VARIABLES/HANDLES

//FILE DIALOG WINDOW GROUP
HWND hFileInstructions;
HWND hFilePathInput;
HWND hFileCancelButton;
HWND hFileFunctionButton;

//MAP EDIT SCREEN GROUP
HMENU hMapEditMenu;
HWND hBitmapHolder;
bool MapEditAddingFreeSpace{ false };
bool MapEditAddingObstacle{ false };
bool MapEditAddingStart{ false };
bool MapEditAddingEnd{ false };
bool MapEditAddingL_Shape{ false };
bool MapEditAddingSmallSquare{ false };
bool MapEditAddingBigSquare{ false };
bool MapEditAddingVLine{ false };
bool MapEditAddingHLine{ false };

//START SCREEN GROUP
HWND hStartMessage;
HWND hOpenButton;
HWND hNewButton;
HWND hHelpButton;

//NEW MAP SCREEN GROUP
HWND hNewMapInstructions;
HWND hNewMapHeightLabel;
HWND hNewMapHeightInput;
HWND hNewMapWidthLabel;
HWND hNewMapWidthInput;
HWND hNewMapCreateButton;