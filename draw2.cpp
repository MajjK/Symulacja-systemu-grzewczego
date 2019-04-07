
#include "stdafx.h"
#include "draw2.h"
#include <vector>
#include <cstdio>
#include <fstream>
#include <string>
#include <iostream>
#include <cmath>
#include <Windows.h>
#include <sstream>


#define MAX_LOADSTRING 100
#define TMR_1 1

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
int ignore, logs, steps, type, Time = 1000;
double gravity, U;
float time_M, time_S;

struct status
{
	double	time_signature = 1;
	double amplitude = 1;
	bool drawX = true;
	bool drawY = true;
	bool drawZ = true;
	bool realtime = false;
}CHART;


struct acceleration
{
	double value;
	bool movement;
};

struct pojemnosc
{	
	std::vector<Point> T;
	double C;
	double R;
};

// buttons
HWND hwndButton;
HWND hwndText0, hwndText1, hwndText2, hwndText3, hwndText4;
// sent data
int col = 0;
std::vector<acceleration> data_acc;
std::vector<Point> data_U;
std::vector<Point> data_y;
std::vector<Point> data_z;
pojemnosc Wall, Chamber;
RECT drawArea1 = { 5, 5, 1660, 1200 };
RECT drawArea2 = { 10, 570, 400, 600 };
// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Buttons(HWND, UINT, WPARAM, LPARAM);

void Input(int type) {
	switch (type) {
		case 0:
			break;
		case 1:
			for (int i = 0; i < Time; i++) {
				data_U.push_back(Point(Time, U * 1));
			}

			break;
		case 2:
			break;
		default:
			break;
	}
}




void calc_gravity() {
	double sum = 0;
	int reject = 0;

	for (int i = ignore; i < logs; i++) {
		if (data_acc[i].movement == false)
			sum = sum + data_acc[i].value;
		else
			reject++;
	}
	gravity = sum / (logs - ignore - reject);
}


void calc_time(int act) {
	float count_M = 0;
	float count_S = 0;
	for (int i = ignore; i <act; i++) {
		if (data_acc[i].movement == true)
			count_M++;
		else count_S++;
	}
	time_M = count_M / 25;
	time_S = count_S / 25;
}


void calc_steps(int act) {
	if ((data_acc[act + 1].movement == true) && (data_acc[act].movement == false))
		steps++;
}


bool check(double value)
{
	if ((value - gravity < -0.115) || (value - gravity > 0.115)) {
		return true;
	}
	else
		return false;
}


void check2() {
	for (int i = 1; i < logs / 10; i++) {
		int number = 0;
		bool movement;

		for (int j = 0; j < 10; j++) {
			if (data_acc[10 * i + j].movement == true)
				number++;

			if (number > 3) {
				movement = true;
			}
			else
				movement = false;
		}

		for (int j = -10; j < 0; j++)
			data_acc[10 * i + j].movement = movement;
	}
}







void MyOnPaint(HDC hdc, status status)
{
	steps = 0;
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 255));
	Pen pen2(Color(255, 255, 0, 0));
	Pen pen3(Color(255, 0, 255, 0));
	Pen chart1(Color(200, 34, 100, 140), 2);
	Pen chart2(Color(200, 34, 100, 140), 1);
	Pen chart3(Color(200, 34, 100, 140), 0.5);
	REAL dashVals[4] = {
		10.0f,   // dash length 5
		8.0f,   // space length 2
		10.0f,  // dash length 15
		8.0f };  // space length 4
	chart2.SetDashPattern(dashVals, 4);
	graphics.DrawLine(&chart1, 5, 5, 5, 840);
	graphics.DrawLine(&chart1, 5, 100 * status.amplitude, 1600, 100 * status.amplitude);
	for (int i = 50; i < 850; i += 50)graphics.DrawLine(&chart2, 5, i, 1600, i);
	for (int i = 5; i < 2300; i += 50*status.time_signature)graphics.DrawLine(&chart2, i, 620, i, 660);
	for (int i = ignore+1; i < 2299; i++) {
		calc_time(i);
		if (i != 2298)calc_steps(i);

// Pobieranie danych do temperatur
		std::ostringstream strs, strs2, strs3, grav, kroki, czas_M, czas_S;
		strs << i / 25;
		std::string str = strs.str();
		std::wstring stemp = std::wstring(str.begin(), str.end());
		LPCWSTR sw = stemp.c_str();

		strs2 << data_acc[i].value;
		std::string str2 = strs2.str();
		std::wstring stemp2 = std::wstring(str2.begin(), str2.end());
		LPCWSTR sw2 = stemp2.c_str();

		strs3 << data_U[100].Y;
		std::string str3 = strs3.str();
		std::wstring stemp3 = std::wstring(str3.begin(), str3.end());
		LPCWSTR sw3 = stemp3.c_str();


// Tu wyswietlone obecne temperatury
		TextOut(hdc, 410, 680, sw, 2);
		TextOut(hdc, 350, 680, TEXT("T1 : "), 6);
		TextOut(hdc, 410, 720, sw2, 6);
		TextOut(hdc, 350, 720, TEXT("T2 : "), 6);

		TextOut(hdc, 500, 640, TEXT("Wejœcie:"), 9);
		TextOut(hdc, 800, 640, TEXT("Zmienna:"), 9);
		TextOut(hdc, 870, 640, sw3, 3);


		if (status.drawY)graphics.DrawLine(&pen2, Point((data_y[i - 1].X - ignore)*status.time_signature+5, data_y[i - 1].Y*status.amplitude), Point((data_y[i].X - ignore)*status.time_signature+5, data_y[i].Y*status.amplitude));
		if (status.drawZ)graphics.DrawLine(&pen3, Point((data_z[i - 1].X - ignore)*status.time_signature+5, data_z[i - 1].Y*status.amplitude), Point((data_z[i].X - ignore) *status.time_signature+5, data_z[i].Y*status.amplitude));
		if (CHART.realtime)Sleep(10);
	}

}


void repaintWindow(HWND hWnd, HDC &hdc, PAINTSTRUCT &ps, RECT *drawArea, status status)
{
	if (drawArea == NULL)
		InvalidateRect(hWnd, NULL, TRUE); // repaint all
	else
		InvalidateRect(hWnd, drawArea, TRUE);

	//repaint drawArea
	hdc = BeginPaint(hWnd, &ps);
	MyOnPaint(hdc, status);
	EndPaint(hWnd, &ps);
}


void getData()
{
	std::ifstream file;
	file.open("outputRobotForwardB02.log");
	double koniec, x, y, z;
	double suma = 0;
	std::string pomin;
	logs = 0;

	while (file.eof() == false) {
		file.seekg(21, std::ios::cur);
		file >> x;
		file >> y;
		file >> z;
		std::getline(file, pomin);

		acceleration log = { sqrt(x*x + y*y + z*z), false };
		data_acc.push_back(log);
	//	data_x.push_back(Point(logs, (abs((x * 1000) - 200)) / 2));
		data_y.push_back(Point(logs, (abs((y * 1000) - 200)) / 2));
		data_z.push_back(Point(logs, (abs((z * 1000) - 200)) / 2));
		logs++;
	}

	calc_gravity();
	for (int i = 0; i < logs; i++)
		data_acc[i].movement = check(data_acc[i].value);

	check2();
	calc_gravity();
	file.close();
}



void Begin()
{
	pojemnosc Wall, Chamber;
	Wall.C = 0;
	Wall.R = 0;
	Chamber.C = 0;
	Chamber.R = 0;

	// Funkcja wymagajaca ustawienia wartosci poczatkowych

}

int OnCreate(HWND window)
{
	getData();
	return 0;
}


// main function (exe hInstance)
int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;
	status status;

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DRAW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);



	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DRAW));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	GdiplusShutdown(gdiplusToken);

	return (int)msg.wParam;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DRAW));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_DRAW);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle (of exe) in our global variable

	// main window
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 300, 1600, 880, NULL, NULL, hInstance, NULL);
                                                     

	hwndButton = CreateWindow(TEXT("button"),
		TEXT("C1"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		20, 670,
		40, 30,
		hWnd,
		(HMENU)ID_BUTTON1,
		hInstance,
		NULL);

	hwndButton = CreateWindow(TEXT("button"),
		TEXT("C2"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		20, 720,
		40, 30,
		hWnd,
		(HMENU)ID_BUTTON2,
		hInstance,
		NULL);

	hwndButton = CreateWindow(TEXT("button"),                    
		TEXT("R1"),                  
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
		175, 670,                                  
		40, 30,                              
		hWnd,                                 
		(HMENU)ID_BUTTON3,                   
		hInstance,                            
		NULL);   

	hwndButton = CreateWindow(TEXT("button"),
		TEXT("R2"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		175, 720,
		40, 30,
		hWnd,
		(HMENU)ID_BUTTON4,
		hInstance,
		NULL);

	hwndButton = CreateWindow(TEXT("button"),
		TEXT("Strumieñ U(t)"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		620, 670,
		100, 30,
		hWnd,
		(HMENU)ID_BUTTON5,
		hInstance,
		NULL);

	hwndButton = CreateWindowEx(NULL, TEXT("button"), TEXT("Wykres oœ X"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 800, 670, 110, 20, hWnd, (HMENU)ID_CHECKBOX1, hInstance, NULL);


	hwndText0 = CreateWindowEx(ES_NUMBER, TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 70, 675, 80, 20,
		hWnd, (HMENU)ID_text1, hInstance, NULL);
	hwndText1 = CreateWindowEx(ES_NUMBER, TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 230, 675, 80, 20,
		hWnd, (HMENU)ID_text2, hInstance, NULL);
	hwndText2 = CreateWindowEx(ES_NUMBER, TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 70, 720, 80, 20,
		hWnd, (HMENU)ID_text3, hInstance, NULL);
	hwndText3 = CreateWindowEx(ES_NUMBER, TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 230, 720, 80, 20,
		hWnd, (HMENU)ID_text4, hInstance, NULL);
	hwndText4 = CreateWindowEx(ES_NUMBER, TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 620, 710, 100, 20,
		hWnd, (HMENU)ID_text5, hInstance, NULL);
	// create button and store the handle                                                       



	hwndButton = CreateWindow(TEXT("button"), TEXT("Prost"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		500, 660, 100, 40, hWnd, (HMENU)ID_RBUTTON0, GetModuleHandle(NULL), NULL);
	hwndButton = CreateWindow(TEXT("button"), TEXT("Skok"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		500, 700, 100, 40, hWnd, (HMENU)ID_RBUTTON1, GetModuleHandle(NULL), NULL);
	hwndButton = CreateWindow(TEXT("button"), TEXT("Sin"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		500, 740, 100, 40, hWnd, (HMENU)ID_RBUTTON2, GetModuleHandle(NULL), NULL);

	OnCreate(hWnd);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window (low priority)
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	LPWSTR Bufor = (LPWSTR)GlobalAlloc(GPTR, 5);
	char bufor[5];

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		// MENU & BUTTON messages
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_BUTTON1:
			GetWindowText(hwndText0, Bufor, 5);
			wcstombs(bufor, Bufor, 5);
			Chamber.C = atof(bufor);
			GlobalFree(Bufor);
			break;
		case ID_BUTTON2:
			GetWindowText(hwndText1, Bufor, 5);
			wcstombs(bufor, Bufor, 5);
			Wall.C = atof(bufor);
			GlobalFree(Bufor);
			break;
		case ID_BUTTON3:
			GetWindowText(hwndText2, Bufor, 5);
			wcstombs(bufor, Bufor, 5);
			Chamber.R = atof(bufor);
			GlobalFree(Bufor);
			break;
		case ID_BUTTON4:
			GetWindowText(hwndText3, Bufor, 5);
			wcstombs(bufor, Bufor, 5);
			Wall.R = atof(bufor);
			GlobalFree(Bufor);
			break;
		case ID_BUTTON5:
			GetWindowText(hwndText4, Bufor, 5);
			wcstombs(bufor, Bufor, 5);
			U = atof(bufor);
			Input(type);
			GlobalFree(Bufor);
			break;
		case ID_BUTTON7:
			GetWindowText(hwndText1, Bufor, 5);
			wcstombs(bufor, Bufor, 5);
			ignore = atoi(bufor);
			GlobalFree(Bufor);
			calc_gravity();
			repaintWindow(hWnd, hdc, ps, NULL, CHART);
			break;
		case ID_CHECKBOX1:
			WM_XBUTTONDOWN;
			if (CHART.drawX)CHART.drawX = false;
			else	CHART.drawX = true;
			repaintWindow(hWnd, hdc, ps, NULL, CHART);
			break;
		case ID_RBUTTON0:
		//	KillTimer(hWnd, TMR_1);
			type=0;
			break;
		case ID_RBUTTON1:
			type=1;

			break;
		case ID_RBUTTON2:
			type=2;

			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here (not depend on timer, buttons)
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_TIMER:
		switch (wParam)
		{
		case TMR_1:
			//force window to repaint
			//	repaintWindow(hWnd, hdc, ps, &drawArea2, CHART);
			CHART.realtime = true;
			break;
		}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}