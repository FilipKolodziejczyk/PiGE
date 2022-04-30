// lab1.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "lab1.h"

#define MAX_LOADSTRING	100
#define BALL_TIMER_ID	1
#define SOLID_MODE		0
#define BIT_MODE		1
#define STRETCH_MODE	2
#define BASIS			10

// Window settings:
const FLOAT		windowTransparency = 0.2f;
const INT		windowWidth = 200;
const INT		windowHeight = 300;
const COLORREF	startBgColor = RGB(255, 255, 0);

// Ball settings;
const INT		ballSize = 15;
const INT		ballFreshRate = 50;
const INT		ballSpeed = 6;
const INT		ballStartX = 40;
const INT		ballStartY = 60;
const INT		ballColor = RGB(255, 0, 0);

// Paddle settings:
const INT		paddleWidth = 60;
const INT		paddleHeight = 15;
const INT		maxDigits = 4;
const COLORREF	textColor = RGB(0, 0, 0);
const HBRUSH	paddleBrush = (HBRUSH)(COLOR_ACTIVECAPTION + 1);

// Global Variables:
HINSTANCE		hInst;                             
WCHAR			szTitle[MAX_LOADSTRING];             
WCHAR			szWindowClass[MAX_LOADSTRING];        
WCHAR			szBallClass[MAX_LOADSTRING];
WCHAR			szPaddleClass[MAX_LOADSTRING];
HWND			hWnd;
HWND			hBall;
HWND			hPaddle;
BOOL			gameOver;
INT				points;
INT				clientWidth;
INT				clientHeight;


// Forward declarations of functions included in this code module:
BOOL				CenterWindow(HWND);
ATOM                RegisterWindowClass(HINSTANCE);
ATOM                RegisterPaddleClass(HINSTANCE);
ATOM                RegisterBallClass(HINSTANCE);
BOOL                InitInstance(HINSTANCE, INT);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    PaddleProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    BallProc(HWND, UINT, WPARAM, LPARAM);

// Application core function
INT APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					  _In_opt_ HINSTANCE hPrevInstance,
					  _In_ LPWSTR    lpCmdLine,
					  _In_ INT       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_LAB1, szWindowClass, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_BALL, szBallClass, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PADDLE, szPaddleClass, MAX_LOADSTRING);

	// Register classes
	RegisterWindowClass(hInstance);
	RegisterBallClass(hInstance);
	RegisterPaddleClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
		return FALSE;

	HACCEL hAccelTable = LoadAcceleratorsW(hInstance, MAKEINTRESOURCE(IDC_LAB1));

	MSG msg;

	// Main message loop:
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		if (!TranslateAcceleratorW(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	return (INT)msg.wParam;
}

BOOL InitInstance(HINSTANCE hInstance, INT nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_COMPOSITED | WS_EX_TOPMOST, szWindowClass, szTitle,
						   WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
						   CW_USEDEFAULT, 0, windowWidth, windowHeight, nullptr, nullptr, hInstance, nullptr);

	hBall = CreateWindowExW(0, szBallClass, nullptr, WS_CHILD, 0, 0,
							ballSize, ballSize, hWnd, nullptr, hInstance, nullptr);

	hPaddle = CreateWindowExW(0, szPaddleClass, nullptr, WS_CHILD, 0, 0,
							  paddleWidth, paddleHeight, hWnd, nullptr, hInstance, nullptr);

	SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), (BYTE)(255 * (1 - windowTransparency)), LWA_ALPHA);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	ShowWindow(hPaddle, nCmdShow);
	UpdateWindow(hPaddle);

	ShowWindow(hBall, nCmdShow);
	UpdateWindow(hBall);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HDC		offDC;
	static HBITMAP	hBitmap;
	static COLORREF bgColor;
	static INT		mode;

	switch (message)
	{
	case WM_CREATE:
	{
		CenterWindow(hWnd);
		RECT rc;
		GetClientRect(hWnd, &rc);
		clientWidth = rc.right - rc.left;
		clientHeight = rc.bottom - rc.top;
		gameOver = FALSE;
		bgColor = startBgColor;
		mode = SOLID_MODE;

		HDC hdc = GetDC(hWnd);
		offDC = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, clientWidth, clientHeight);
		SelectObject(offDC, hBitmap);
		ReleaseDC(hWnd, hdc);
	}
	break;

	case WM_MOUSEMOVE:
	{
		if (gameOver)
			break;

		INT xPos = GET_X_LPARAM(lParam);
		if (xPos + paddleWidth > clientWidth)
			xPos = clientWidth - paddleWidth;
		MoveWindow(hPaddle, xPos, clientHeight - paddleHeight, paddleWidth, paddleHeight, TRUE);
	}
	break;

	case WM_COMMAND:
	{
		INT wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

		case IDM_RESTART:
		{
			gameOver = FALSE;
			points = 0;
			InvalidateRect(hPaddle, NULL, TRUE);
			SendMessageW(hBall, WM_APP, 0, 0);
		}
		break;

		case IDM_COLOR:
		{
			CHOOSECOLOR cc;
			static COLORREF arcCustClr[16];
			ZeroMemory(&cc, sizeof(cc));
			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = hWnd;
			cc.lpCustColors = arcCustClr;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc))
			{
				bgColor = cc.rgbResult;
				HMENU hMenu = GetMenu(hWnd);
				EnableMenuItem(hMenu, IDM_TILE, MF_GRAYED);
				EnableMenuItem(hMenu, IDM_STRETCH, MF_GRAYED);
				mode = SOLID_MODE;
				InvalidateRect(hWnd, NULL, TRUE);
			}
		}
		break;

		case IDM_BITMAP:
		{
			OPENFILENAME ofn;
			wchar_t szFile[260];
			HANDLE hf;

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = szFile;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = TEXT("Bitmap\0*.bmp\0");
			ofn.nFilterIndex = 1;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetOpenFileNameW(&ofn))
			{
				hBitmap = (HBITMAP)LoadImageW(NULL, ofn.lpstrFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

				HMENU hMenu = GetMenu(hWnd);
				EnableMenuItem(hMenu, IDM_TILE, MF_ENABLED);
				EnableMenuItem(hMenu, IDM_STRETCH, MF_ENABLED);
				CheckMenuItem(hMenu, IDM_TILE, MF_CHECKED);
				if (mode == SOLID_MODE)
					mode = BIT_MODE;
				InvalidateRect(hWnd, NULL, TRUE);
			}
		}
		break;

		case IDM_TILE:
		{
			HMENU hMenu = GetMenu(hWnd);
			CheckMenuItem(hMenu, IDM_TILE, MF_CHECKED);
			CheckMenuItem(hMenu, IDM_STRETCH, MF_UNCHECKED);
			mode = BIT_MODE;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

		case IDM_STRETCH:
		{
			HMENU hMenu = GetMenu(hWnd);
			CheckMenuItem(hMenu, IDM_TILE, MF_UNCHECKED);
			CheckMenuItem(hMenu, IDM_STRETCH, MF_CHECKED);
			mode = STRETCH_MODE;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

		default:
			return DefWindowProcW(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		RECT rc;
		GetClientRect(hWnd, &rc);

		switch (mode)
		{
		case SOLID_MODE:
			FillRect(hdc, &rc, CreateSolidBrush(bgColor));
			break;

		case BIT_MODE:
		{
			HBRUSH hBrush = CreatePatternBrush(hBitmap);
			FillRect(offDC, &rc, hBrush);
			DeleteBrush(hBrush);
			BitBlt(hdc, 0, 0, clientWidth, clientHeight, offDC, 0, 0, SRCCOPY);
		}
		break;

		case STRETCH_MODE:
		{
			HDC tempDC = CreateCompatibleDC(hdc);
			SelectObject(tempDC, hBitmap);
			BITMAP bitmap;
			GetObject(hBitmap, sizeof(BITMAP), &bitmap);
			BitBlt(hdc, 0, 0, clientWidth, clientHeight, tempDC, 0, 0, SRCCOPY);
			StretchBlt(hdc, 0, 0, clientWidth, clientHeight, tempDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
			DeleteDC(tempDC);
		}
		break;
		}

		EndPaint(hWnd, &ps);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK PaddleProc(HWND hPaddle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		points = 0;
		MoveWindow(hPaddle, 0, clientHeight - paddleHeight, paddleWidth, paddleHeight, TRUE);
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hPaddle, &ps);
		wchar_t pointsStr[maxDigits + 1];
		_itow_s(points, pointsStr, BASIS);
		pointsStr[maxDigits] = '\0';
		RECT rc;
		GetClientRect(hPaddle, &rc);
		SetTextColor(hdc, textColor);
		SetBkMode(hdc, TRANSPARENT);
		DrawTextW(hdc, pointsStr, (int)_tcslen(pointsStr), &rc, DT_CENTER);
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProcW(hPaddle, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK BallProc(HWND hBall, UINT message, WPARAM wParam, LPARAM lParam)
{
	static INT x;
	static INT y;
	static INT x_dir = 1;
	static INT y_dir = 1;

	switch (message)
	{
	case WM_CREATE:
	{
		HRGN hRgn = CreateEllipticRgn(0, 0, ballSize, ballSize);
		SetWindowRgn(hBall, hRgn, true);
		x = ballStartX;
		y = ballStartY;
		MoveWindow(hBall, x, y, ballSize, ballSize, TRUE);
		SetTimer(hBall, BALL_TIMER_ID, ballFreshRate, NULL);
	}
	break;

	case WM_APP:
	{
		x = ballStartX;
		y = ballStartY;
		x_dir = y_dir = 1;
		MoveWindow(hBall, x, y, ballSize, ballSize, TRUE);
		SetTimer(hBall, BALL_TIMER_ID, ballFreshRate, NULL);
	}
	break;

	case WM_TIMER:
	{
		if (y + ballSize + y_dir < ballSize)
		{
			// ceil bounce
			y_dir *= -1;
		}
		else if (y + ballSize + y_dir + paddleHeight > clientHeight)
		{
			RECT rc;
			GetWindowRect(hPaddle, &rc);
			MapWindowRect(HWND_DESKTOP, hWnd, &rc);

			if (x + ballSize > rc.left && x < rc.right)
			{
				// paddle bounce
				points++;
				y_dir *= -1;
				InvalidateRect(hPaddle, NULL, TRUE);
			}
			else
			{
				// game over
				KillTimer(hBall, BALL_TIMER_ID);
				gameOver = TRUE;
				break;
			}
		}
		y += y_dir * ballSpeed;

		if (x + ballSize + x_dir > clientWidth || x + ballSize + x_dir < ballSize)
		{
			// side bounce
			x_dir *= -1;
		}
		x += x_dir * ballSpeed;

		MoveWindow(hBall, x, y, ballSize, ballSize, TRUE);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProcW(hBall, message, wParam, lParam);
	}
	return 0;
}

BOOL CenterWindow(HWND hWnd)
{
	RECT rc;
	if (!SystemParametersInfoW(SPI_GETWORKAREA, 0, &rc, 0))
		return FALSE;

	INT midX = (rc.right - rc.left) / 2;
	INT midY = (rc.bottom - rc.top) / 2;

	if (!GetWindowRect(hWnd, &rc))
		return FALSE;

	INT height = rc.bottom - rc.top;
	INT width = rc.right - rc.left;
	if (!MoveWindow(hWnd, midX - width / 2, midY - height / 2, width, height, TRUE))
		return FALSE;

	return TRUE;
}

ATOM RegisterWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex = {};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_LAB1));
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIconW(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

ATOM RegisterPaddleClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex = {};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = PaddleProc;
	wcex.hInstance = hInstance;
	wcex.hbrBackground = paddleBrush;
	wcex.lpszClassName = szPaddleClass;

	return RegisterClassExW(&wcex);
}

ATOM RegisterBallClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex = {};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = BallProc;
	wcex.hInstance = hInstance;
	wcex.hbrBackground = CreateSolidBrush(ballColor);
	wcex.lpszClassName = szBallClass;

	return RegisterClassExW(&wcex);
}