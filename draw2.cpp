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

HINSTANCE hInst;								
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
int type, Time = 1700;
double U;

struct status
{
	double	time_signature = 1;
	double amplitude = 1;
	bool drawX = true;
	bool drawY = true;
	bool drawZ = true;
	bool realtime = false;
}CHART;

struct pojemnosc
{	
	std::vector<Point> T;
	double C;
	double R;
};


HWND hwndButton;
HWND hwndText0, hwndText1, hwndText2, hwndText3, hwndText4;


std::vector<Point> data_U;
pojemnosc Wall, Chamber, Environment;
RECT drawArea1 = { 5, 5, 1660, 1200 };
RECT drawArea2 = { 10, 570, 400, 600 };

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Buttons(HWND, UINT, WPARAM, LPARAM);

//Mozna to poprawic ale dziala
double integration(std::vector<Point> Function, int i) {

	double integral = 0, acc = 1000, h = i / acc;

	for (int j = 1; j < acc; j++)
	{
		integral = integral + Function[j * h].Y;
	}

	integral = integral + Function[0].Y / 2;
	integral = integral + Function[i].Y / 2;
	integral = integral * h;

	return integral;
}

// Te dwie funkcje jeszcze niedokonczone, dzieja sie dziwne rzeczy
int Heat_flow(Point T1, Point T2, double R) {
	return ((T1.Y - T2.Y) / R);
}


void calculateTemp() {

	std::vector<Point> Q_1, Q_2;

	for (int i = 0; i < Time - 10; i++) {
		Q_1.push_back(Point(i, Heat_flow(Chamber.T[i], Wall.T[i], Chamber.R)));
		Q_2.push_back(Point(i, Heat_flow(Wall.T[i], Environment.T[i], Chamber.R)));

		Chamber.T.push_back(Point(i, ((1 / Chamber.C) * (integration(data_U, i) - integration(Q_1, i)))));
		Wall.T.push_back(Point(i, ((1 / Wall.C) * (integration(Q_1, i) - integration(Q_2, i)))));
	}

}


void Input(int type) {
	
	data_U.clear();


	switch (type) {

		case 0:  // problem z rysowaniem, w forze jest i - 10
			for (int i = 0, k = 0; i < Time; i++) {
				if ((i >= k*100) && (i < k*100 + 100)) {
					data_U.push_back(Point(i, U));
				}
				else if ( i % 100 == 0)
				k += 2;
				else{	
					data_U.push_back(Point(i, 0));
				}
			}
/*			for (int i = 0; i < Time - 10; i++) 
				data.push_back(Point(i, integration(data_U , i)));*/
			break;

		case 1:
			for (int i = 0; i < Time; i++) 
				data_U.push_back(Point(i, U));
			break;

		case 2:
			for (int i = 0; i < Time; i++) 
				data_U.push_back(Point(i, (U/2)*sin(i/31.847) + U/2));
			break;

		default:
			break;
	}
}


void MyOnPaint(HDC hdc, status status)
{
	Graphics graphics(hdc);
	Pen pen1(Color(255, 0, 0, 255));
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
	for (int i = 1; i < Time - 10; i++) {
	

// Pobieranie danych do temperatur
		std::ostringstream strs, strs2, strs3;
		strs << i / 25;
		std::string str = strs.str();
		std::wstring stemp = std::wstring(str.begin(), str.end());
		LPCWSTR sw = stemp.c_str();
/*
		strs2 << data_acc[i].value;
		std::string str2 = strs2.str();
		std::wstring stemp2 = std::wstring(str2.begin(), str2.end());
		LPCWSTR sw2 = stemp2.c_str();
		*/
		strs3 << data_U[250].Y;
		std::string str3 = strs3.str();
		std::wstring stemp3 = std::wstring(str3.begin(), str3.end());
		LPCWSTR sw3 = stemp3.c_str();


// Tu obecne temperatury
		TextOut(hdc, 410, 680, sw, 2);
		TextOut(hdc, 350, 680, TEXT("T1 : "), 6);
	//	TextOut(hdc, 410, 720, sw2, 6);
		TextOut(hdc, 350, 720, TEXT("T2 : "), 6);

		TextOut(hdc, 500, 640, TEXT("Wejście:"), 9);
		TextOut(hdc, 800, 640, TEXT("Zmienna:"), 9);
		TextOut(hdc, 870, 640, sw3, 3);

    	if (status.drawX)graphics.DrawLine(&pen1, Point((data_U[i - 1].X), (-1) * data_U[i - 1].Y + 100), Point((data_U[i].X),  (-1) * data_U[i].Y + 100));
	    if (status.drawY)graphics.DrawLine(&pen2, Point((Chamber.T[i - 1].X ), (-1) * Chamber.T[i - 1].Y + 350), Point((Chamber.T[i].X), (-1) * Chamber.T[i].Y + 350));
		if (status.drawZ)graphics.DrawLine(&pen3, Point((Wall.T[i - 1].X), (-1) * Wall.T[i - 1].Y + 600), Point((Wall.T[i].X), (-1) * Wall.T[i].Y + 600));
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


void Begin()
{
	Wall.C = 0;
	Wall.R = 0;
	Chamber.C = 0;
	Chamber.R = 0;
	Wall.T.push_back (Point(0,0));
	Chamber.T.push_back(Point(0,0));

	for (int i = 0; i < Time; i++) {
		Environment.T.push_back(Point(i, 0));
	}
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
		TEXT("Strumień U(t)"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		620, 670,
		100, 30,
		hWnd,
		(HMENU)ID_BUTTON5,
		hInstance,
		NULL);

	hwndButton = CreateWindowEx(NULL, TEXT("button"), TEXT("Wykresy"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 800, 670, 110, 20, hWnd, (HMENU)ID_CHECKBOX1, hInstance, NULL);


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



	hwndButton = CreateWindow(TEXT("button"), TEXT("Prost"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		500, 660, 100, 40, hWnd, (HMENU)ID_RBUTTON0, GetModuleHandle(NULL), NULL);
	hwndButton = CreateWindow(TEXT("button"), TEXT("Skok"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		500, 700, 100, 40, hWnd, (HMENU)ID_RBUTTON1, GetModuleHandle(NULL), NULL);
	hwndButton = CreateWindow(TEXT("button"), TEXT("Sin"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		500, 740, 100, 40, hWnd, (HMENU)ID_RBUTTON2, GetModuleHandle(NULL), NULL);

	Begin();

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
		case ID_CHECKBOX1:
	//		WM_XBUTTONDOWN;  do usuniecia ?
			//Tutaj funkcje wyliczajace temperatury t1 i t2




			calculateTemp();
			repaintWindow(hWnd, hdc, ps, NULL, CHART);
			break;
		case ID_RBUTTON0:
			type=0;
			Input(type);
			break;
		case ID_RBUTTON1:
			type=1;
			Input(type);
			break;
		case ID_RBUTTON2:
			type=2;
			Input(type);
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
