// Trees.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "Trees.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

PIXELFORMATDESCRIPTOR pfd = {
	sizeof (PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
	1,											// Version Number
	PFD_DRAW_TO_WINDOW |						// Format Must Support Window
	PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
	PFD_DOUBLEBUFFER,							// Must Support Double Buffering
	PFD_TYPE_RGBA,								// Request An RGBA Format
	32,										// Select Our Color Depth
	0, 0, 0, 0, 0, 0,							// Color Bits Ignored
	0,											// No Alpha Buffer
	0,											// Shift Bit Ignored
	0,											// No Accumulation Buffer
	0, 0, 0, 0,									// Accumulation Bits Ignored
	16,											// 16Bit Z-Buffer (Depth Buffer)  
	0,											// No Stencil Buffer
	0,											// No Auxiliary Buffer
	PFD_MAIN_PLANE,								// Main Drawing Layer
	0,											// Reserved
	0, 0, 0										// Layer Masks Ignored
};

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass (HINSTANCE hInstance);
BOOL				InitInstance (HINSTANCE, int);
LRESULT CALLBACK	WndProc (HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About (HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain (_In_ HINSTANCE hInstance,
						_In_opt_ HINSTANCE hPrevInstance,
						_In_ LPTSTR    lpCmdLine,
						_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER (hPrevInstance);
	UNREFERENCED_PARAMETER (lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString (hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString (hInstance, IDC_TREES, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass (hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) {
		return FALSE;
	}

	hAccelTable = LoadAccelerators (hInstance, MAKEINTRESOURCE (IDC_TREES));

	// Main message loop:
	while (GetMessage (&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator (msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass (HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof (WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_TREES));
	wcex.hCursor = LoadCursor (NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = MAKEINTRESOURCE (IDC_TREES);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon (wcex.hInstance, MAKEINTRESOURCE (IDI_SMALL));

	return RegisterClassEx (&wcex);
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
BOOL InitInstance (HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	HDC  hdc;
	HGLRC hglrc;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow (szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
						 CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd) return FALSE;

	hdc = GetDC (hWnd);
	SetPixelFormat (hdc, ChoosePixelFormat(hdc,&pfd), &pfd);
	hglrc = wglCreateContext (hdc);
	wglMakeCurrent (hdc, hglrc);

	ShowWindow (hWnd, nCmdShow);
	UpdateWindow (hWnd);

	return TRUE;
}

void ResizeGLScene (GLsizei width, GLsizei height)		// Resize/Initialize The GL Window
{
	if (height == 0) height = 1;

	glViewport (0, 0, width, height);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f); // Calculate The Aspect Ratio Of The Window
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}

int DrawGLScene ()
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity ();
	Trees t (1,1,0.01,100);
	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) {
	case WM_COMMAND:
		wmId = LOWORD (wParam);
		wmEvent = HIWORD (wParam);
		// Parse the menu selections:
		switch (wmId) {
		case IDM_ABOUT:
			DialogBox (hInst, MAKEINTRESOURCE (IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow (hWnd);
			break;
		default:
			return DefWindowProc (hWnd, message, wParam, lParam);
		}
		break;
	case WM_CREATE:
		glShadeModel (GL_SMOOTH);
		glClearColor (0.0f, 0.0f, 0.0f, 0.5f);
		glClearDepth (1.0f);
		glEnable (GL_DEPTH_TEST);
		glDepthFunc (GL_LEQUAL);
		glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		break;
	case WM_SIZE:
		ResizeGLScene (LOWORD (lParam), HIWORD (lParam));
		break;
	case WM_TIMER:
	case WM_PAINT:
		hdc = BeginPaint (hWnd, &ps);
		DrawGLScene ();
		SwapBuffers (hdc);
		EndPaint (hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage (0);
		break;
	default:
		return DefWindowProc (hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER (lParam);
	switch (message) {
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD (wParam) == IDOK || LOWORD (wParam) == IDCANCEL) {
			EndDialog (hDlg, LOWORD (wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}