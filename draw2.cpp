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
int Time = 32000, type = 0;
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
	std::vector<double> T;
	double C;
	double R;
};


HWND hwndButton;
HWND hwndText0, hwndText1, hwndText2, hwndText3, hwndText4;


std::vector<double> data_U;
pojemnosc Wall, Chamber, Environment;
RECT drawArea1 = { 5, 5, 1660, 1200 };
RECT drawArea2 = { 10, 570, 400, 600 };

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Buttons(HWND, UINT, WPARAM, LPARAM);

std::vector<double> integration(std::vector<double> Function , int i) {

	std::vector<double> integral;

	integral.push_back(0);

	for (int j = i ; j < Function.size() ; j++)
	{
		integral.push_back(integral[j-1] + (Function[j - 1] + Function[j])/2) ;
	}

	return integral;
}


void calculateTemp() {

	std::vector<double> Q1, Q2, U, IQ1, IQ2;
	U = integration(data_U, 1);

	Chamber.T.clear();
	Wall.T.clear();
	Chamber.T.push_back(0);
	Wall.T.push_back(0);

	Q1.clear();
	IQ1.clear();
	Q1.push_back(0);
	IQ1.push_back(0);
	Q2.clear();
	IQ2.clear();
	Q2.push_back(0);
	IQ2.push_back(0);


	for (int i = 1; i < Time - 20; i++) {

		Q1.push_back((Chamber.T[i - 1] - Wall.T[i - 1]) / Chamber.R);
		IQ1.push_back(IQ1[i - 1] + (Q1[i - 1] + Q1[i]) / 2);

		Q2.push_back((Wall.T[i - 1] - Environment.T[i - 1]) / Wall.R);
		IQ2.push_back(IQ2[i - 1] + (Q2[i - 1] + Q2[i]) / 2);

		Chamber.T.push_back((U[i] - IQ1[i]) / Chamber.C);
		Wall.T.push_back((IQ1[i] - IQ2[i]) / Wall.C);
	}
}


void Input(int type) {
	
	data_U.clear();

	switch (type) {

		case 0:  
			for (int i = 0, k = 0; i < Time; i++) {
				if ((i >= k*1000) && (i < k*1000 + 1000)) {
					data_U.push_back(U);
				}
				else if ( i % 1000 == 0)
				k += 2;
				else{	
					data_U.push_back(0);
				}
			}
			break;

		case 1:
			for (int i = 0; i < Time; i++) 
				data_U.push_back(U);		
			break;

		case 2:
			for (int i = 0; i < Time; i++) 
				data_U.push_back((U/2)*sin(i/318.47) + U/2);
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
		graphics.DrawLine(&chart1, 5, 400 * status.amplitude, 1600, 400 * status.amplitude);
		graphics.DrawLine(&chart1, 5, 600 * status.amplitude, 1600, 600 * status.amplitude);
		for (int i = 50; i < 850; i += 50)graphics.DrawLine(&chart2, 5, i, 1600, i);
//      for (int i = 5; i < 2300; i += 50*status.time_signature)graphics.DrawLine(&chart2, i, 620, i, 660);
	for (int i = 1; i < (Time - 30); i++) {


		// Pobieranie danych do wyswietlenia
		std::ostringstream strs, strs2, strs3;
		
		strs << i/1000;
		std::string str = strs.str();
		std::wstring stemp = std::wstring(str.begin(), str.end());
		LPCWSTR sw = stemp.c_str();
		
		strs2 << Environment.T[0];
		std::string str2 = strs2.str();
		std::wstring stemp2 = std::wstring(str2.begin(), str2.end());
		LPCWSTR sw2 = stemp2.c_str();


		//Napisy
		TextOut(hdc, 430, 680, sw, 2);
		TextOut(hdc, 340, 680, TEXT("Czas:"), 6);
		TextOut(hdc, 430, 720, sw2, 2);
		TextOut(hdc, 340, 720, TEXT("Temp otocz:"), 12);


		TextOut(hdc, 500, 640, TEXT("Pobudzenie:"), 12);

		TextOut(hdc, 10, 5, TEXT("U (T):"), 6);
		TextOut(hdc, 10, 105, TEXT("T1 (T):"), 7);
		TextOut(hdc, 10, 405, TEXT("T2 (T):"), 7);

		if (status.drawX)graphics.DrawLine(&pen1, Point((i - 1)/20 + 5 , -1 * data_U[i-1] + 100), Point(i/20 + 5 , -1 * data_U[i] + 100));
		if (status.drawY)graphics.DrawLine(&pen2, Point((i - 1)/20 + 5 , -1 * Chamber.T[i-1] + 400), Point(i/20 + 5 , -1 * Chamber.T[i] + 400));
	 	if (status.drawZ)graphics.DrawLine(&pen3, Point((i - 1)/20 + 5 , -1 * Wall.T[i-1] + 600), Point(i/20 + 5, -1 * Wall.T[i] + 600));
		 //if (CHART.realtime)Sleep(10);
	}
}


void repaintWindow(HWND hWnd, HDC &hdc, PAINTSTRUCT &ps, RECT *drawArea, status status)
{
	if (drawArea == NULL)
		InvalidateRect(hWnd, NULL, TRUE); // repaint all
	else
		InvalidateRect(hWnd, drawArea, TRUE);

	hdc = BeginPaint(hWnd, &ps);
	MyOnPaint(hdc, status);
	EndPaint(hWnd, &ps);
}


void Begin()
{
	Wall.C = 2;
	Wall.R = 4;
	Chamber.C = 80;
	Chamber.R = 13;
	U = 10;
	Wall.T.push_back (0);
	Chamber.T.push_back(0);

	for (int i = 0; i < Time; i++) {
		Environment.T.push_back(0);
	}
}


// main function 
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



	// application initialization:
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


// Registers the window class.

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

//Saves instance handle and creates main window

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; 

	// main window
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 300, 1600, 880, NULL, NULL, hInstance, NULL);
                                                     

	hwndButton = CreateWindow(TEXT("button"),
		TEXT("C1"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		20, 670,
		40, 30,
		hWnd,
		(HMENU)ID_BUTTON0,
		hInstance,
		NULL);

	hwndButton = CreateWindow(TEXT("button"),
		TEXT("C2"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		20, 720,
		40, 30,
		hWnd,
		(HMENU)ID_BUTTON1,
		hInstance,
		NULL);

	hwndButton = CreateWindow(TEXT("button"),                    
		TEXT("R1"),                  
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
		175, 670,                                  
		40, 30,                              
		hWnd,                                 
		(HMENU)ID_BUTTON2,                   
		hInstance,                            
		NULL);   

	hwndButton = CreateWindow(TEXT("button"),
		TEXT("R2"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		175, 720,
		40, 30,
		hWnd,
		(HMENU)ID_BUTTON3,
		hInstance,
		NULL);

	hwndButton = CreateWindow(TEXT("button"),
		TEXT("Strumień U(t)"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		620, 670,
		100, 30,
		hWnd,
		(HMENU)ID_BUTTON4,
		hInstance,
		NULL);

	hwndButton = CreateWindowEx(NULL, TEXT("button"), TEXT("Wykresy"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 800, 670, 110, 20, hWnd, (HMENU)ID_CHECKBOX1, hInstance, NULL);

	//mozna dodac do okienek biezace wartosci parametrow oraz wczytywanie wartosci z 1 przycisku
	hwndText0 = CreateWindowEx(ES_NUMBER, TEXT("EDIT"), TEXT("80"), WS_CHILD | WS_VISIBLE | WS_BORDER, 70, 675, 80, 20,
		hWnd, (HMENU)ID_text1, hInstance, NULL);
	hwndText1 = CreateWindowEx(ES_NUMBER, TEXT("EDIT"), TEXT("13"), WS_CHILD | WS_VISIBLE | WS_BORDER, 70, 720, 80, 20,
		hWnd, (HMENU)ID_text2, hInstance, NULL);    
	hwndText2 = CreateWindowEx(ES_NUMBER, TEXT("EDIT"), TEXT("2"), WS_CHILD | WS_VISIBLE | WS_BORDER, 230, 675, 80, 20,
		hWnd, (HMENU)ID_text3, hInstance, NULL);
	hwndText3 = CreateWindowEx(ES_NUMBER, TEXT("EDIT"), TEXT("4"), WS_CHILD | WS_VISIBLE | WS_BORDER, 230, 720, 80, 20,
		hWnd, (HMENU)ID_text4, hInstance, NULL);
	hwndText4 = CreateWindowEx(ES_NUMBER, TEXT("EDIT"), TEXT("10"), WS_CHILD | WS_VISIBLE | WS_BORDER, 620, 710, 100, 20,
		hWnd, (HMENU)ID_text5, hInstance, NULL);                                                     


	//mozna dodac automatyczne zaznaczenie 1 kropki
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
		case ID_BUTTON0:
			GetWindowText(hwndText0, Bufor, 5);
			wcstombs(bufor, Bufor, 5);
			Chamber.C = atof(bufor);
			GlobalFree(Bufor);
			break;
		case ID_BUTTON1:
			GetWindowText(hwndText1, Bufor, 5);
			wcstombs(bufor, Bufor, 5);
			Wall.C = atof(bufor);
			GlobalFree(Bufor);
			break;
		case ID_BUTTON2:
			GetWindowText(hwndText2, Bufor, 5);
			wcstombs(bufor, Bufor, 5);
			Chamber.R = atof(bufor);
			GlobalFree(Bufor);
			break;
		case ID_BUTTON3:
			GetWindowText(hwndText3, Bufor, 5);
			wcstombs(bufor, Bufor, 5);
			Wall.R = atof(bufor);
			GlobalFree(Bufor);
			break;
		case ID_BUTTON4:
			GetWindowText(hwndText4, Bufor, 5);
			wcstombs(bufor, Bufor, 5);
			U = atof(bufor);
			Input(type);
			GlobalFree(Bufor);
			break;
		case ID_CHECKBOX1:
			calculateTemp();
			Input(type);
			repaintWindow(hWnd, hdc, ps, NULL, CHART);
			break;
		case ID_RBUTTON0:
			type = 0;
			Input(type);
			break;
		case ID_RBUTTON1:
			type = 1;
			Input(type);
			break;
		case ID_RBUTTON2:
			type = 2;
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
