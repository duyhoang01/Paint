// Bai10.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Bai10.h"
#include <stdio.h>
#include "Winuser.h"
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

// Define Constant Variables 
#pragma region DEFINE
#define MAX_LOADSTRING 100
#define COLOR_RED RGB(255, 0, 0)
#define COLOR_ORANGE RGB(255, 128, 64)
#define COLOR_YELLOW RGB(255, 255, 0)
#define COLOR_GREEN RGB(0, 255, 0)
#define COLOR_BLUE RGB(0, 0, 255)
#define COLOR_BLACK RGB(0, 0, 0)
#define COLOR_GRAY RGB(128, 128, 128)
#define COLOR_WHITE RGB(255, 255, 255)
#define COLOR_PURPLE RGB(163, 73, 164)
#define COLOR_PINK RGB(255, 128, 192)
#pragma endregion

// Default Variables and functions
#pragma region DefaultVariablesAndFunctions
// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
												// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
#pragma endregion

// khai bao bien toan cuc
#pragma region KhaiBaoBien
// toolbox
HWND hwndTool;
HWND hWnd;
//HWND hDlg;

int left, right, top, bottom;
LPRECT rc;
//Rect rc;
POINT point[55000];
HBRUSH hbrWhite, hbrGray;
HPEN hPen1, hPen2; // bút vẽ dùng trong tool: pencil và eraser và shape.
HBRUSH hbrushMauNen; // dùng để đổ màu và tính năng fill của shape. 
// Lưu 2 giá trị màu sắc: màu vẽ và màu nền. 
COLORREF colorDraw = COLOR_BLACK;
COLORREF colorSec = COLOR_WHITE;
COLORREF cursorColor;
bool isColorDraw = true;
// open and save

//cursor
HCURSOR hCursor = LoadCursor(hInst, MAKEINTRESOURCE(CUR_PENCIL));

int n = 0;
// lấy các giá trị hiện tại. 
int firstTools = -1;
int firstTool = -1;

int cursorTools = TOOLS_TOOL;        //lay id cua loai cong cu hien tai. (Pencil or Eraser or Brush)
int cursorTool = TOOL_PENCIL;        //lay id cua cong cu hien tai. (Pencil - Fill color - Eraser - Color picker)
int cursorShape = -1;                // lay id cua hinh (Line - Rectangle - Elipse ...vv)
// Size
int sizeIndex = 1;
int cursorSize = 1;                  // lay SIZE hiện tại
// flag
bool isDraw = false;
bool isFill = false;  // 
bool isChangeCursor = false;
/* -----------------   Edit color  ----------------------*/
COLORREF acrCustClr[16];
COLORREF color = RGB(0, 0, 0);
COLORREF colorTmp1, colorTmp2;
int indexTmp = -1;
HICON IconTmp1, IconTmp2;
// file
PAINTSTRUCT ps;
HBITMAP       hBitmap, hOldBitmap;
HPALETTE      hPalette, hOldPalette;
HDC           hMemDC, hdc, hdc1;
BITMAP        bm;
OPENFILENAME ofn;       // common dialog box structure
HANDLE hf;              // file handle
TCHAR lpstrFileName[_MAX_PATH];

#pragma endregion

// ham tu dinh nghia
#pragma region CacHamTuDinhNghia
// cac phuong thuc về công cụ - thiet lap con tro chuot
void processToolShape(HWND hWnd, int wmId);
void processToolPencil(HWND hWnd);
void processToolEraser(HWND hWnd);
void processToolFillColor(HWND hWnd);
void processToolColorPicker(HWND hWnd);
void setCursorPosition(HWND hWnd, int Cur);
void chooseCursor(HWND hWnd, bool isColorPicker = true);
// cac phuong thuc tao but va choi
void setPen(HDC hdc, COLORREF colorDraw);
void setBrush(HDC hdc, COLORREF colorSec);
// cac phuong thuc ve mau sac. 
void floodFill(int x, int y, COLORREF oldColor, COLORREF newColor);
void EditColor(HWND hWnd);
void changeColor(HWND hDlg, COLORREF color);
HICON CreateSolidColorIcon(COLORREF iconColor, int width, int height);
// cac phuong thuc ve tool 
void drawPencil(HWND hWnd, HDC hdc);
// cac phuong thuc ve hinh
void drawShape(HWND hWnd, HDC hdc, int cursorShape);
void drawLine(HWND hWnd, HDC hdc);
void drawTriangle(HWND hWnd, HDC hdc);
void drawStarIV(HWND hWnd, HDC hdc);
void drawStarV(HWND hWnd, HDC hdc);
void drawStarVI(HWND hWnd, HDC hdc);
void Diamond(HWND hWnd, HDC hdc);
// cac phuong thuc mo va luu file
void OpenFile(HWND hWnd);
void SaveFile(HWND hWnd);
// set kich thuoc cua line and point
void setSize(int cursorTools, int cursorTool, int size = sizeIndex);

// hien thi dialog toolbox
INT_PTR CALLBACK    ToolBox(HWND, UINT, WPARAM, LPARAM);
void setIcon(HWND hDlg, int ICON, int BUTTON, int size = 1);
void setIconForButtonColor(HWND hDlg, int BUTTON, COLORREF colour, int size = 1);
#pragma endregion

// các hàm khởi tạo mặc định của hệ thống. 
#pragma region CacHamKhoiTaoMacDinhCuaHeThong
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_BAI10, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BAI10));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ICON_APPPAINT));
	wcex.hCursor = LoadCursor(nullptr, MAKEINTRESOURCE(CUR_PENCIL));
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_BAI10);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(ICON_APPPAINT));

	return RegisterClassExW(&wcex);
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		127, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}
	hwndTool = CreateDialog(hInst, MAKEINTRESOURCE(DLG_TOOL), hWnd, ToolBox);
	ShowWindow(hWnd, nCmdShow);
	ShowWindow(hwndTool, TRUE);
	UpdateWindow(hWnd);

	return TRUE;
}
#pragma endregion
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE: {
		setSize(cursorTools, cursorTool, 1);
		processToolPencil(hWnd);
		chooseCursor(hWnd, false);
		
		break; }
	case WM_LBUTTONDOWN: {
		left = LOWORD(lParam);
		top = HIWORD(lParam);
		isDraw = true;
		if (cursorTools == TOOLS_TOOL && (cursorTool == TOOL_PENCIL || cursorTool == TOOL_ERASER)) {
			setPen(hdc, colorDraw);
			setBrush(hdc, colorSec);
			point[0].x = left;
			point[0].y = top;

		}
		else if (cursorTools == TOOLS_TOOL && cursorTool == TOOL_FILLC0LOR) {
			cursorColor = GetPixel(hdc, left, top);
			floodFill(left, top, cursorColor, colorDraw);
		}
		else if (cursorTools == TOOLS_SHAPE) {
			point[0].x = left;
			point[0].y = top;
		}
		break;
	}
	case WM_LBUTTONUP: {
		hdc = GetDC(hWnd);
		if (cursorTools == TOOLS_TOOL && (cursorTool == TOOL_PENCIL || cursorTool == TOOL_ERASER)) isDraw = false;
		else if (cursorTools == TOOLS_TOOL && cursorTool == TOOL_COLORPICKER) {
			colorDraw = GetPixel(hdc, LOWORD(lParam), HIWORD(lParam));
			changeColor(hwndTool, colorDraw);
			chooseCursor(hWnd);
			isChangeCursor = true;
			break;
		}
		if (cursorTools == TOOLS_SHAPE) {
			right = LOWORD(lParam);
			bottom = HIWORD(lParam);
			point[n].x = LOWORD(lParam);
			point[n].y = HIWORD(lParam);
			setPen(hdc, colorDraw);
			setBrush(hdc, colorSec);
			if (isDraw) { drawShape(hWnd, hdc, cursorShape); isDraw = false; }
			break;
		}
		break;
	}
	case WM_MOUSEMOVE: {
		// set cursor
		if (isChangeCursor) {
			chooseCursor(hWnd, false);
			isChangeCursor = false;
		}
		n++;
		point[n].x = LOWORD(lParam);
		point[n].y = HIWORD(lParam);
		
		hdc = GetDC(hWnd);
		SelectObject(hdc, hPen1);
		if (isDraw) {
			if (cursorTools == TOOLS_TOOL && cursorTool == TOOL_PENCIL) {
				point[1].x = LOWORD(lParam);
				point[1].y = HIWORD(lParam);
				Polygon(hdc, point, 2);
				ReleaseDC(hWnd, hdc);
				point[0].x = point[1].x;
				point[0].y = point[1].y;
			}
			else if (cursorTools == TOOLS_TOOL && cursorTool == TOOL_ERASER) {
				point[1].x = LOWORD(lParam);
				point[1].y = HIWORD(lParam);
				hPen2 = CreatePen(PS_SOLID, cursorSize, colorSec);
				SelectObject(hdc, hPen2);
				Polygon(hdc, point, 2);
				ReleaseDC(hWnd, hdc);
				point[0].x = point[1].x;
				point[0].y = point[1].y;
			}
			else if (cursorTools == TOOLS_BRUSH) {
				//drawPencil(hWnd);
			}
			else if (cursorTools == TOOLS_SHAPE) {
				//if (isDraw) { drawShape(hWnd, hdc, cursorShape); isDraw = false; }
			}
		}

		break;
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		int wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
			// menu  file
#pragma region MenuFile
		case IDM_NEW:   InvalidateRect(hWnd, NULL, TRUE); break;
		case IDM_OPEN: 	OpenFile(hWnd);  break;
		case IDM_SAVE:	SaveFile(hWnd);	break;
		case ID_EDITCOLOR:	EditColor(hWnd);	break;
		case IDM_ABOUT:   DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);   break;
		case IDM_EXIT:    DestroyWindow(hWnd);    break;
#pragma endregion
			// thiet lap gia tri cua size
#pragma region ThietLapSize
		case ID_SIZE1: setSize(cursorTools, cursorTool, 1); sizeIndex = 1; break;
		case ID_SIZE2: setSize(cursorTools, cursorTool, 2); sizeIndex = 2; break;
		case ID_SIZE3: setSize(cursorTools, cursorTool, 3); sizeIndex = 3; break;
		case ID_SIZE4: setSize(cursorTools, cursorTool, 4); sizeIndex = 4; break;
#pragma endregion
			// flag fill
#pragma region flag fill
		case ID_FILL_FILL: isFill = true; break;
		case ID_FILL_NOFILL: isFill = false; break;
#pragma endregion

		default:   return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}
	
	case WM_DESTROY: {
		PostQuitMessage(0);
		break; }
	default:    return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// định nghĩa các hàm
#pragma region DinhNghiaCacHam
// các hàm vẽ hình. 
#pragma region CacPhuongThuVeVeHinh
void setPoint() {
	left = point[0].x;		top = point[0].y;
	right = point[n].x;		bottom = point[n].y;
}
void drawPencil(HWND hWnd, HDC hdc) {
	MoveToEx(hdc, point[n - 1].x, point[n - 1].y, NULL);
	LineTo(hdc, point[n].x, point[n].y);
	ReleaseDC(hWnd, hdc);
}
void drawShape(HWND hWnd, HDC hdc, int cursorShape) {
	switch (cursorShape)
	{
	case B_LINE: drawLine(hWnd, hdc);		break;
	case B_TRIANGLE:	drawTriangle(hWnd, hdc);	break;
	case B_ELIPSE:Ellipse(hdc, left, top, right, bottom);	break;
	case B_RECTANGLE:Rectangle(hdc, left, top, right, bottom);	break;
		//RoundRect(hdc, left, top, right, bottom, 50, 50); break;
	case B_FOURSTAR:drawStarIV(hWnd, hdc);	break;
	case B_FIVESTAR:drawStarV(hWnd, hdc);	break;
	case B_SIXSTAR:drawStarVI(hWnd, hdc);	break;
	case B_DIAMOND:Diamond(hWnd, hdc);	break;
	}
}
void drawLine(HWND hWnd, HDC hdc) {
	static POINT Double[2];
	setPoint();
	Double[0].x = left;
	Double[0].y = top;

	Double[1].x = right;
	Double[1].y = bottom;

	Polyline(hdc, Double, 2);
	ReleaseDC(hWnd, hdc);
}
void drawTriangle(HWND hWnd, HDC hdc) {
	static POINT trippel[3];
	//setPoint();
	trippel[0].x = left + (right - left) / 2;					trippel[0].y = top;
	trippel[1].x = left;										trippel[1].y = bottom;
	trippel[2].x = right;										trippel[2].y = bottom;

	Polygon(hdc, trippel, 3);
	ReleaseDC(hWnd, hdc);
}
void drawStarIV(HWND hWnd, HDC hdc) {
	point[0].x = left;
	point[0].y = top + (bottom - top) / 2;

	point[1].x = left + 3 * (right - left) / 8;
	point[1].y = top + 4 * (bottom - top) / 10;

	point[2].x = left + (right - left) / 2;
	point[2].y = top;

	point[3].x = left + 5 * (right - left) / 8;
	point[3].y = top + 4 * (bottom - top) / 10;

	point[4].x = right;
	point[4].y = top + (bottom - top) / 2;

	point[5].x = left + 5 * (right - left) / 8;
	point[5].y = top + 6 * (bottom - top) / 10;

	point[6].x = left + (right - left) / 2;
	point[6].y = bottom;

	point[7].x = left + 3 * (right - left) / 8;
	point[7].y = top + 6 * (bottom - top) / 10;

	Polygon(hdc, point, 8);
	ReleaseDC(hWnd, hdc);
}
void drawStarV(HWND hWnd, HDC hdc) {
	point[0].x = left;
	point[0].y = top + 2 * (bottom - top) / 5;

	point[1].x = left + 38 * (right - left) / 100 - 2;
	point[1].y = top + 2 * (bottom - top) / 5 - 3;

	point[2].x = left + (right - left) / 2;
	point[2].y = top;

	point[3].x = left + 62 * (right - left) / 100 + 3;
	point[3].y = top + 2 * (bottom - top) / 5 - 3;

	point[4].x = right;
	point[4].y = top + 2 * (bottom - top) / 5;

	point[5].x = left + 7 * (right - left) / 10 - 2;
	point[5].y = top + 3 * (bottom - top) / 5;

	point[6].x = left + 8 * (right - left) / 10;
	point[6].y = bottom;

	point[7].x = left + (right - left) / 2;
	point[7].y = top + 385 * (bottom - top) / 500;

	point[8].x = left + 2 * (right - left) / 10;
	point[8].y = bottom;

	point[9].x = left + 3 * (right - left) / 10 + 2;
	point[9].y = top + 3 * (bottom - top) / 5;

	Polygon(hdc, point, 10);
	ReleaseDC(hWnd, hdc);
}
void drawStarVI(HWND hWnd, HDC hdc) {
	static POINT Six[12];
	Six[0].x = left;								Six[0].y = top + (bottom - top) / 4;
	Six[1].x = left + (right - left) / 3;			Six[1].y = top + (bottom - top) / 4;
	Six[2].x = left + (right - left) / 2;			Six[2].y = top;
	Six[3].x = left + 2 * (right - left) / 3;		Six[3].y = top + (bottom - top) / 4;
	Six[4].x = right;								Six[4].y = top + (bottom - top) / 4;
	Six[5].x = left + 5 * (right - left) / 6;		Six[5].y = top + (bottom - top) / 2;
	Six[6].x = right;								Six[6].y = top + 3* (bottom - top) / 4;
	Six[7].x = left + 2*(right - left) / 3;			Six[7].y = top + 3* (bottom - top) / 4;
	Six[8].x = left + (right - left) / 2;			Six[8].y = bottom;
	Six[9].x = left + +(right - left) / 3;			Six[9].y = top + 3* (bottom - top) / 4;
	Six[10].x = left;								Six[10].y = top + 3* (bottom - top) / 4;
	Six[11].x = left + (right - left) / 6;			Six[11].y = top + (bottom - top) / 2;

	Polygon(hdc, Six, 12);
	ReleaseDC(hWnd, hdc);
}
void Diamond(HWND hWnd, HDC hdc) {
	static POINT Dia[4];
	Dia[0].x = (left + right) / 2;				Dia[0].y = top;
	Dia[1].x = right;							Dia[1].y = (top + bottom) / 2;
	Dia[2].x = (left + right) / 2;				Dia[2].y = bottom;
	Dia[3].x = left;							Dia[3].y = (top + bottom) / 2;
	Polygon(hdc, Dia, 4);
	ReleaseDC(hWnd, hdc);
}
#pragma endregion
// hàm xử lý file
#pragma region CacPhuongThucXuLyFile
BOOL LoadBitmapFromBMPFile(LPTSTR szFileName, HBITMAP *phBitmap, HPALETTE *phPalette)
{
	BITMAP  bm;

	*phBitmap = NULL;
	*phPalette = NULL;

	// Use LoadImage() to get the image loaded into a DIBSection
	*phBitmap = (HBITMAP)LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0,LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
	if (*phBitmap == NULL) return FALSE;

	// Get the color depth of the DIBSection
	GetObject(*phBitmap, sizeof(BITMAP), &bm);
	// If the DIBSection is 256 color or less, it has a color table
	if ((bm.bmBitsPixel * bm.bmPlanes) <= 8)
	{
		HDC           hMemDC;
		HBITMAP       hOldBitmap;
		RGBQUAD       rgb[256];
		LPLOGPALETTE  pLogPal;
		WORD          i;

		// Create a memory DC and select the DIBSection into it
		hMemDC = CreateCompatibleDC(NULL);
		hOldBitmap = (HBITMAP)SelectObject(hMemDC, *phBitmap);
		// Get the DIBSection's color table
		GetDIBColorTable(hMemDC, 0, 256, rgb);
		// Create a palette from the color tabl
		pLogPal = (LOGPALETTE *)malloc(sizeof(LOGPALETTE) + (256 * sizeof(PALETTEENTRY)));
		pLogPal->palVersion = 0x300;
		pLogPal->palNumEntries = 256;
		for (i = 0; i<256; i++)
		{
			pLogPal->palPalEntry[i].peRed = rgb[i].rgbRed;
			pLogPal->palPalEntry[i].peGreen = rgb[i].rgbGreen;
			pLogPal->palPalEntry[i].peBlue = rgb[i].rgbBlue;
			pLogPal->palPalEntry[i].peFlags = 0;
		}
		*phPalette = CreatePalette(pLogPal);
		// Clean up
		free(pLogPal);
		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
	}
	else   // It has no color table, so use a halftone palette
	{
		HDC    hRefDC;

		hRefDC = GetDC(NULL);
		*phPalette = CreateHalftonePalette(hRefDC);
		ReleaseDC(NULL, hRefDC);
	}
	return TRUE;

}
void OpenFile(HWND hWnd) {
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = lpstrFileName;
	ofn.lpstrFile[0] = L'\0';
	ofn.nMaxFile = sizeof(lpstrFileName);
	ofn.lpstrFilter = L"Bitmap Files(*.bmp;*.dib)\0*.bmp;*.dib\0JPEG (*jpg;*jpeg;*jpe;*jfif)\0*jpg;*jpeg;*jpe;*jfif*\0GIF\0*.gif*\0TIFF(*.tif;*.tiff)\0*.tif;*.tiff*\0PNG(*.png)\0*.png*\0ICO (*.ico)\0*.ico*\0All Picture Files\0*.jpg;*.png;*.jpeg;*.bmp;*.gif\0All Files\0*.*\0";
	ofn.nFilterIndex = 7;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn)) {
		hdc1 = BeginPaint(hWnd, &ps);
		if (LoadBitmapFromBMPFile(lpstrFileName, &hBitmap, &hPalette))
		{
			GetObject(hBitmap, sizeof(BITMAP), &bm);
			hMemDC = CreateCompatibleDC(hdc1);
			hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
			hOldPalette = SelectPalette(hdc1, hPalette, FALSE);
			RealizePalette(hdc1);

			BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight,hMemDC, 0, 0, SRCCOPY);

			SelectObject(hMemDC, hOldBitmap);
			DeleteObject(hBitmap);
			SelectPalette(hdc1, hOldPalette, FALSE);
			DeleteObject(hPalette);
		}
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
}
int CaptureAnImage(HWND hWnd, LPTSTR szFileName)
{
	HDC hdcScreen;
	HDC hdcWindow;
	HDC hdcMemDC = NULL;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;

	// Retrieve the handle to a display device context for the client 
	// area of the window. 
	hdcScreen = GetDC(NULL);
	hdcWindow = GetDC(hWnd);

	// Create a compatible DC which is used in a BitBlt from the window DC
	hdcMemDC = CreateCompatibleDC(hdcWindow);

	if (!hdcMemDC)
	{
		MessageBox(hWnd, L"CreateCompatibleDC has failed", L"Failed", MB_OK);
		DeleteObject(hbmScreen);
		DeleteObject(hdcMemDC);
		ReleaseDC(NULL, hdcScreen);
		ReleaseDC(hWnd, hdcWindow);
	}

	// Get the client area for size calculation
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	//This is the best stretch mode
	SetStretchBltMode(hdcWindow, HALFTONE);

	//The source DC is the entire screen and the destination DC is the current window (HWND)
	if (!StretchBlt(hdcWindow, 0, 0, rcClient.right, rcClient.bottom, hdcScreen, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SRCCOPY))
	{
		MessageBox(hWnd, L"StretchBlt has failed", L"Failed", MB_OK);
		DeleteObject(hbmScreen);
		DeleteObject(hdcMemDC);
		ReleaseDC(NULL, hdcScreen);
		ReleaseDC(hWnd, hdcWindow);
	}

	// Create a compatible bitmap from the Window DC
	hbmScreen = CreateCompatibleBitmap(hdcWindow, rcClient.right, rcClient.bottom );

	if (!hbmScreen)
	{
		MessageBox(hWnd, L"CreateCompatibleBitmap Failed", L"Failed", MB_OK);
		DeleteObject(hbmScreen);
		DeleteObject(hdcMemDC);
		ReleaseDC(NULL, hdcScreen);
		ReleaseDC(hWnd, hdcWindow);
	}

	// Select the compatible bitmap into the compatible memory DC.
	SelectObject(hdcMemDC, hbmScreen);

	// Bit block transfer into our compatible memory DC.
	if (!BitBlt(hdcMemDC, 0, 0, 1000, 800, hdcWindow, 0, 0, SRCCOPY))
	{
		MessageBox(hWnd, L"BitBlt has failed", L"Failed", MB_OK);
		DeleteObject(hbmScreen);
		DeleteObject(hdcMemDC);
		ReleaseDC(NULL, hdcScreen);
		ReleaseDC(hWnd, hdcWindow);
	}

	// Get the BITMAP from the HBITMAP
	GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

	BITMAPFILEHEADER   bmfHeader;
	BITMAPINFOHEADER   bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpScreen.bmWidth;
	bi.biHeight = bmpScreen.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

	// Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
	// call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
	// have greater overhead than HeapAlloc.
	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
	char *lpbitmap = (char *)GlobalLock(hDIB);

	// Gets the "bits" from the bitmap and copies them into a buffer 
	// which is pointed to by lpbitmap.
	GetDIBits(hdcWindow, hbmScreen, 0, (UINT)bmpScreen.bmHeight, lpbitmap, (BITMAPINFO *)&bi, DIB_RGB_COLORS);

	// A file is created, this is where we will save the screen capture.
	HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	// Add the size of the headers to the size of the bitmap to get the total file size
	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//Offset to where the actual bitmap bits start.
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

	//Size of the file
	bmfHeader.bfSize = dwSizeofDIB;

	//bfType must always be BM for Bitmaps
	bmfHeader.bfType = 0x4D42; //BM   

	DWORD dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

	//Unlock and Free the DIB from the heap
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	//Close the handle for the file that was created
	CloseHandle(hFile);

	//Clean up
	DeleteObject(hbmScreen);
	DeleteObject(hdcMemDC);
	ReleaseDC(NULL, hdcScreen);
	ReleaseDC(hWnd, hdcWindow);

	return 0;
}
void SaveFile(HWND hWnd) {
	static HBITMAP hBmp;
	static PBITMAPINFO pbi;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = lpstrFileName;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(lpstrFileName);
	ofn.lpstrFilter = L"Bitmap Files(*.bmp;*.dib)\0*.bmp;*.dib\0JPEG (*jpg;*jpeg;*jpe;*jfif)\0*jpg;*jpeg;*jpe;*jfif*\0GIF\0*.gif*\0TIFF(*.tif;*.tiff)\0*.tif;*.tiff*\0PNG(*.png)\0*.png*\0ICO (*.ico)\0*.ico*\0All Picture Files\0*.jpg;*.png;*.jpeg;*.bmp;*.gif\0All Files\0*.*\0";
	ofn.nFilterIndex = 7;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	if (GetSaveFileName(&ofn)) {
		CaptureAnImage(hWnd, lpstrFileName);
		/*hdc1 = BeginPaint(hWnd, &ps);
		pbi = CreateBitmapInfoStruct(hBmp);
		CreateBMPFile(lpstrFileName, hBmp);
		EndPaint(hWnd, &ps);*/
	}
}


#pragma endregion
// hàm lựa chọn size và color
#pragma region CacPhuongThucVeColorVaSize

void floodFill(int x, int y, COLORREF oldColor, COLORREF newColor) {
	if (oldColor == GetPixel(hdc, x, y))
	{
		SetPixel(hdc, x, y, newColor);
		floodFill(x + 1, y, oldColor, newColor);
		floodFill(x, y + 1, oldColor, newColor);
		floodFill(x - 1, y, oldColor, newColor);
		floodFill(x, y - 1, oldColor, newColor);
	}
	return;
}

void changeColor(HWND hDlg, COLORREF color) {
	if (isColorDraw) {
		(colorDraw = color);
		setIconForButtonColor(hDlg, B_DRAWCOLOR, color, 2);
	}
	else {
		(colorSec = color);
		setIconForButtonColor(hDlg, B_SECCOLOR, color);
	}
}
void selectColor(HWND hDlg, int id) {
	switch (id)
	{
	case C_RED: changeColor(hDlg, COLOR_RED); break;
	case C_BLACK:changeColor(hDlg, COLOR_BLACK); break;
	case C_GREEN:changeColor(hDlg, COLOR_GREEN); break;
	case C_BLUE:changeColor(hDlg, COLOR_BLUE); break;
	case C_WHITE:changeColor(hDlg, COLOR_WHITE); break;
	case C_GRAY:changeColor(hDlg, COLOR_GRAY); break;
	case C_PINK:changeColor(hDlg, COLOR_PINK); break;
	case C_ORANGE:changeColor(hDlg, COLOR_ORANGE); break;
	case C_PURPLE:changeColor(hDlg, COLOR_PURPLE); break;
	case C_YELLOW: changeColor(hDlg, COLOR_YELLOW); break;
	case C_TMP1: changeColor(hDlg, colorTmp1); break;
	case C_TMP2: changeColor(hDlg, colorTmp2); break;
	}
}
void EditColor(HWND hWnd)
{
	CHOOSECOLOR chooseColor = { sizeof(CHOOSECOLOR) };
	chooseColor.Flags = CC_RGBINIT | CC_FULLOPEN | CC_ANYCOLOR;
	chooseColor.hwndOwner = hWnd;
	chooseColor.lpCustColors = acrCustClr;
	chooseColor.rgbResult = color;
	if (ChooseColor(&chooseColor))
	{
		color = chooseColor.rgbResult;
		//colorDraw = chooseColor.rgbResult;
	}
}
void setSize(int cursorTools, int cursorTool, int size) {
	if (cursorTools == TOOLS_SHAPE || (cursorTools == TOOLS_TOOL && cursorTool == TOOL_PENCIL)) cursorSize = size;
	else if(cursorTools == TOOLS_TOOL && cursorTool == TOOL_ERASER) cursorSize = 2 * size + 2;
	else {
		if (cursorTool == BRUSH_BRUSH) { 
			if (size == 1) cursorSize = 1; 
			else if (size == 2) cursorSize = 3;
			else if (size == 3) cursorSize = 5;
			else if (size == 4) cursorSize = 8;
		}
		if (cursorTool == BRUSH_AIRBRUSH) (size < 3) ? cursorSize = 4*size : cursorSize = 8 * (size - 1);
		if (cursorTool == BRUSH_OILBRUSH)(size < 3) ? cursorSize = 8 * size : cursorSize = 10 * size;
	}
}
HICON CreateSolidColorIcon(COLORREF iconColor, int width, int height)
{
	// Obtain a handle to the screen device context.
	HDC hdcScreen = GetDC(NULL);

	// Create a memory device context, which we will draw into.
	HDC hdcMem = CreateCompatibleDC(hdcScreen);

	// Create the bitmap, and select it into the device context for drawing.
	HBITMAP hbmp = CreateCompatibleBitmap(hdcScreen, width, height);
	HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, hbmp);

	// Draw your icon.
	// 
	// For this simple example, we're just drawing a solid color rectangle
	// in the specified color with the specified dimensions.
	HPEN hpen = CreatePen(PS_SOLID, 1, iconColor);
	HPEN hpenOld = (HPEN)SelectObject(hdcMem, hpen);
	HBRUSH hbrush = CreateSolidBrush(iconColor);
	HBRUSH hbrushOld = (HBRUSH)SelectObject(hdcMem, hbrush);
	Rectangle(hdcMem, 0, 0, width, height);
	SelectObject(hdcMem, hbrushOld);
	SelectObject(hdcMem, hpenOld);
	DeleteObject(hbrush);
	DeleteObject(hpen);

	// Create an icon from the bitmap.
	// 
	// Icons require masks to indicate transparent and opaque areas. Since this
	// simple example has no transparent areas, we use a fully opaque mask.
	HBITMAP hbmpMask = CreateCompatibleBitmap(hdcScreen, width, height);
	ICONINFO ii;
	ii.fIcon = TRUE;
	ii.hbmMask = hbmpMask;
	ii.hbmColor = hbmp;
	HICON hIcon = CreateIconIndirect(&ii);
	DeleteObject(hbmpMask);

	// Clean-up.
	SelectObject(hdcMem, hbmpOld);
	DeleteObject(hbmp);
	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdcScreen);

	// Return the icon.
	return hIcon;
}
#pragma endregion
// cac phuong thuc tao but va choi
#pragma region CacPhuongThucLuaChonButVaChoi
void setPen(HDC hdc, COLORREF colorDraw) {
	hPen1 = CreatePen(PS_SOLID, cursorSize, colorDraw);
	SelectObject(hdc, hPen1);
}
void setBrush(HDC hdc, COLORREF colorSec) {
	if (isFill)	hbrushMauNen = CreateSolidBrush(colorSec);
	else hbrushMauNen = (HBRUSH)GetStockObject(NULL_BRUSH);
	SelectObject(hdc, hbrushMauNen);
}
#pragma endregion
// cac phuong thuc tao con tro chuot cho tool
#pragma region CacPhuongThucThietLapConTroChoTool
void processToolShape(HWND hWnd, int wmId) {
	firstTools = TOOLS_SHAPE;
	firstTool = -1;
	cursorShape = wmId;
	cursorTools = TOOLS_SHAPE;
	setSize(cursorTools, cursorTool, sizeIndex);
}
void processToolPencil(HWND hWnd) {
	firstTools = TOOLS_TOOL;
	firstTool = TOOL_PENCIL;
	cursorTools = TOOLS_TOOL;
	cursorTool = TOOL_PENCIL;
	setSize(cursorTools, cursorTool, sizeIndex);
}
void processToolEraser(HWND hWnd) {
	firstTools = TOOLS_TOOL;
	firstTool = TOOL_ERASER;
	cursorTools = TOOLS_TOOL;
	cursorTool = TOOL_ERASER;
	setSize(cursorTools, cursorTool, sizeIndex);
}
void processToolFillColor(HWND hWnd) {
	cursorTools = TOOLS_TOOL;
	cursorTool = TOOL_FILLC0LOR;
}
void processToolColorPicker(HWND hWnd) {
	cursorTools = TOOLS_TOOL;
	cursorTool = TOOL_COLORPICKER;
}
void setCursorPosition(HWND hWnd, int Cur) {
	hCursor = LoadCursor(hInst, MAKEINTRESOURCE(Cur));
	SetClassLong(hWnd, GCL_HCURSOR, (LONG)(hCursor));
}
void chooseCursor(HWND hWnd, bool isColorPiker) {
	if (isColorPiker) {
		cursorTools = firstTools;
		if (cursorTools != TOOLS_SHAPE) {
			if (firstTool == TOOL_ERASER) { processToolEraser(hWnd); setCursorPosition(hWnd, CUR_ERASER); }
			else if (firstTool == TOOL_PENCIL) { processToolPencil(hWnd); setCursorPosition(hWnd, CUR_PENCIL); }
			else if (firstTool == TOOL_FILLC0LOR) { processToolFillColor(hWnd); setCursorPosition(hWnd, CUR_FILLCOLOR); }
		}
		else { processToolShape(hWnd, cursorShape); setCursorPosition(hWnd, CUR_SHAPE); }
	}
	else {
		if (cursorTools != TOOLS_SHAPE) {
			if (cursorTool == TOOL_ERASER) { processToolEraser(hWnd); setCursorPosition(hWnd, CUR_ERASER); }
			else if (cursorTool == TOOL_PENCIL) { processToolPencil(hWnd); setCursorPosition(hWnd, CUR_PENCIL); }
			else if (cursorTool == TOOL_FILLC0LOR) { processToolFillColor(hWnd); setCursorPosition(hWnd, CUR_FILLCOLOR); }
			else if (cursorTool == TOOL_COLORPICKER) { processToolColorPicker(hWnd); setCursorPosition(hWnd, CUR_COLORPICKER); }
		}
		else { processToolShape(hWnd, cursorShape); setCursorPosition(hWnd, CUR_SHAPE); }
	}
}
#pragma endregion

#pragma region CacPhuongthucDialogToolbox
INT_PTR CALLBACK ToolBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HICON tmp;
	UNREFERENCED_PARAMETER(lParam);
	int id = LOWORD(wParam);
	switch (message)
	{
	case WM_INITDIALOG: {
		setIcon(hDlg, ICON_PENCIL, B_PENCIL);
		setIcon(hDlg, ICON_ERASER, B_ERASER);
		setIcon(hDlg, ICON_FILLCOLOR, B_FILLCOLOR);
		setIcon(hDlg, ICON_COLORPICKER, B_COLORPICKER);

		setIcon(hDlg, ICON_LINE, B_LINE);
		setIcon(hDlg, ICON_TRIANGLE, B_TRIANGLE);
		setIcon(hDlg, ICON_RECTANGLE, B_RECTANGLE);
		setIcon(hDlg, ICON_ELIPSE, B_ELIPSE);
		setIcon(hDlg, ICON_FOURSTAR, B_FOURSTAR);
		setIcon(hDlg, ICON_FIVESTAR, B_FIVESTAR);
		setIcon(hDlg, ICON_SIXSTAR, B_SIXSTAR);
		setIcon(hDlg, ICON_DIAMOND, B_DIAMOND);

		setIconForButtonColor(hDlg, C_RED, COLOR_RED);
		setIconForButtonColor(hDlg, C_WHITE, COLOR_WHITE);
		setIconForButtonColor(hDlg, C_GRAY, COLOR_GRAY);
		setIconForButtonColor(hDlg, C_GREEN, COLOR_GREEN);
		setIconForButtonColor(hDlg, C_BLACK, COLOR_BLACK);
		setIconForButtonColor(hDlg, C_ORANGE, COLOR_ORANGE);
		setIconForButtonColor(hDlg, C_YELLOW, COLOR_YELLOW);
		setIconForButtonColor(hDlg, C_BLUE, COLOR_BLUE);
		setIconForButtonColor(hDlg, C_PURPLE, COLOR_PURPLE);
		setIconForButtonColor(hDlg, C_PINK, COLOR_PINK);

		setIconForButtonColor(hDlg, B_DRAWCOLOR, COLOR_BLACK, 2);
		setIconForButtonColor(hDlg, B_SECCOLOR, COLOR_WHITE);
		setIcon(hDlg, ICON_EDITCOLOR, B_EDITCOLOR, 3);
		return (INT_PTR)TRUE;
	}
	case WM_COMMAND:
		switch (id)
		{
#pragma region TOOL
		case B_PENCIL: processToolPencil(hWnd); isChangeCursor = true;  break;
		case B_ERASER: processToolEraser(hWnd); isChangeCursor = true; break;
		case B_FILLCOLOR: processToolFillColor(hWnd); isChangeCursor = true; break;
		case B_COLORPICKER: processToolColorPicker(hWnd); isChangeCursor = true; break;
#pragma endregion

#pragma region Shape
		case B_LINE:
		case B_TRIANGLE:
		case B_RECTANGLE:
		case B_ELIPSE:
		case B_FOURSTAR:
		case B_FIVESTAR: 
		case B_SIXSTAR:
		case B_DIAMOND: processToolShape(hWnd, id); isChangeCursor = true; break;
#pragma endregion

#pragma region Color
		case C_RED:
		case C_BLACK:
		case C_GREEN:
		case C_BLUE:
		case C_WHITE:
		case C_GRAY:
		case C_PINK:
		case C_ORANGE:
		case C_PURPLE:
		case C_YELLOW: selectColor(hDlg, id); break;

		case C_TMP1: 
		case C_TMP2:if (indexTmp > -1) selectColor(hDlg, id); break;

		case B_DRAWCOLOR: isColorDraw = true; break;
		case B_SECCOLOR: isColorDraw = false; break;

		case B_EDITCOLOR: {
			EditColor(hWnd);  
			changeColor(hDlg, color);

			if (indexTmp < 1) {
				if (indexTmp == -1) {
					setIconForButtonColor(hDlg, C_TMP1, color); 
					indexTmp = 0; 
					colorTmp1 = color;
				}
				else if (indexTmp == 0) { 
					setIconForButtonColor(hDlg, C_TMP2, color); 
					indexTmp = -1; 
					colorTmp2 = color;
				}
			}

			break;
		}
#pragma endregion
		}
		break;
	}
	return (INT_PTR)FALSE;
}
void setIcon(HWND hDlg, int ICON, int BUTTON, int size) {
	HICON hIcon = (HICON)LoadImageW(GetModuleHandleW(NULL),
		MAKEINTRESOURCEW(ICON), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON)*size, GetSystemMetrics(SM_CYSMICON)*size, 0);
	if (hIcon)
		SendMessage(GetDlgItem(hDlg, BUTTON), BM_SETIMAGE, ICON_BIG, (LPARAM)hIcon);
}
void setIconForButtonColor(HWND hDlg, int BUTTON, COLORREF colour, int size) {
	HICON hIcon = CreateSolidColorIcon(colour, 32*size, 32 * size);
	if (hIcon) SendMessage(GetDlgItem(hDlg, BUTTON), BM_SETIMAGE, ICON_BIG, (LPARAM)hIcon);
}
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
#pragma endregion

#pragma endregion



