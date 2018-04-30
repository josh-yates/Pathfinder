#include "program_definitions.h"

//-----VARIABLES-----

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


//-----FUNCTIONS-----

void FalseAllMapEdits() {
	//set all map editing trackers to false
	MapEditAddingFreeSpace = false;
	MapEditAddingL_Shape = false;
	MapEditAddingObstacle = false;
	MapEditAddingSmallSquare = false;
	MapEditAddingStart = false;
	MapEditAddingEnd = false;
	MapEditAddingBigSquare = false;
	MapEditAddingVLine = false;
	MapEditAddingHLine = false;
}

bool CheckTextPosInt(HWND hWnd, const std::wstring input_string, const int min_val, const int max_val, int& int_out) {
	//remove end of line and spaces with wstringstreams
	std::wstringstream remove_eol(input_string);
	wchar_t buff_peek;
	while ((buff_peek = static_cast<wchar_t>(remove_eol.peek())) == L' ') {
		remove_eol.ignore(1, L'\n');
	}
	//remove the end of line, if present, then put it back on
	std::wstring work_string;
	std::getline(remove_eol, work_string);
	std::wstringstream input_stream(work_string + L'\n');
	double input_double;
	//try to put stream into the double
	input_stream >> input_double;
	//check if it fails, if so, clear the failbit and empty the buffer
	if (input_stream.fail()) {
		input_stream.clear(); input_stream.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
		MessageBox(hWnd, L"Inputs must be integer numbers", L"Invalid input", MB_ICONERROR);
		return false;
	}
	//else, if anything followed the number, fail
	else if ((buff_peek = static_cast<wchar_t>(input_stream.peek())) != L'\n') {
		input_stream.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
		MessageBox(hWnd, L"Inputs must be integer numbers", L"Invalid input", MB_ICONERROR);
		return false;
	}
	//else, it is a number, now check it is >= min_val & is an integer
	else if (input_double < min_val || input_double>max_val) {
		std::wstring range_msg{ L"Inputs must be in range " + std::to_wstring(min_val) + L" to " + std::to_wstring(max_val) };
		MessageBox(hWnd, range_msg.c_str(), L"Invalid input", MB_ICONERROR);
		return false;
	}
	else {
		double int_part, fract_part;
		fract_part = modf(input_double, &int_part);
		if (fract_part == 0) {
			//it is an integer, convert and pass out
			int_out = static_cast<int>(int_part);
			return true;
		}
		else {
			MessageBox(hWnd, L"Inputs must be integer numbers", L"Invalid input", MB_ICONERROR);
			return false;
		}
	}
}

int CalculateScale(const int int_in, const int max_size) {
	double double_scale{ static_cast<double>(max_size) / static_cast<double>(int_in) };
	double rounded_double{ round(double_scale) };
	return static_cast<int>(rounded_double);
}

bool SaveMapToFile(HWND hWnd, base_map& map_in, std::wstring file_path) {
	//check file extension
	size_t find_dot{ file_path.find(L".") };
	std::wstringstream fname_stream;
	fname_stream << file_path;
	if (find_dot != std::wstring::npos) {
		//check extension
		if (file_path.substr(find_dot) != L".txt") {
			MessageBox(hWnd, L"Invalid file name/extension", L"Error", MB_ICONERROR);
			return false;
		}
	}
	else {
		//no extension, so add .txt
		fname_stream << L".txt";
	}
	std::ofstream file_stream{ fname_stream.str() };
	//check file exists
	if (!file_stream) {
		MessageBox(hWnd, L"Unable to access file", L"Error", MB_ICONERROR);
		return false;
	}
	file_stream << map_in;
	file_stream.close();
	MessageBox(hWnd, L"Map saved successully", L"File saved", MB_ICONINFORMATION);
	return true;
}

bool OpenMapFromFile(HWND hWnd, base_map& map_in, std::wstring file_path) {
	//check file extension
	size_t find_dot{ file_path.find(L".") };
	std::wstringstream fname_stream{ file_path };
	if (find_dot != std::wstring::npos) {
		//check extension
		if (file_path.substr(find_dot) != L".txt") {
			MessageBox(hWnd, L"Invalid file name/extension", L"Error", MB_ICONERROR);
			return false;
		}
	}
	else {
		//else no extension, so it is invalid
		MessageBox(hWnd, L"Invalid file name/extension", L"Error", MB_ICONERROR);
		return false;
	}
	//check file exists
	std::ifstream file_stream{ fname_stream.str() };
	if (!file_stream) {
		MessageBox(hWnd, L"Unable to open file", L"Error", MB_ICONERROR);
		file_stream.close();
		return false;
	}
	//catch the errors thrown by the stream operator
	try {
		file_stream >> map_in;
		file_stream.close();
		//check rows = cols, else map was edited
		if (map_in.get_cols() != map_in.get_rows()) {
			MessageBox(hWnd, L"Invalid map file", L"Error", MB_ICONERROR);
			return false;
		}
		//check if map is too big/small
		else if (map_in.get_rows() - 2 > MaxMapSize || map_in.get_cols() - 2 > MaxMapSize) {
			MessageBox(hWnd, L"Map too large", L"Error", MB_ICONERROR);
			return false;
		}
		else if (map_in.get_rows() - 2 < MinMapSize || map_in.get_cols() - 2 < MinMapSize) {
			MessageBox(hWnd, L"Map too small", L"Error", MB_ICONERROR);
			return false;
		}
		else {
			return true;
		}
	}
	catch (const std::invalid_argument&) {
		file_stream.close();
		MessageBox(hWnd, L"Invalid map file", L"Error", MB_ICONERROR);
		return false;
	}
}