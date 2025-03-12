
// Copyright (c) 2025 Ki(y)arash Salehi

// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
// to permit persons to whom the Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of 
// the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.



#define USE_WINDOW_STRUCT_

#include "./header/defWindow.h"

#include <shellapi.h>

#define WM_FCREATE (WM_USER + 1)

#define MAX_CLASS_NAME 512

static LRESULT windowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

Window* GFCreateWindow(
	const WindowConfigure* 				windowInfo, 
	const WindowEvent* 					windowEvent, 
	Window* 							share)
{
	WindowStruct* _window = calloc(1, sizeof(WindowStruct));

	{
		memset(&_window->win32, 0, sizeof(_window->win32));
	}
	if(windowEvent)
		memcpy(&_window->wndEvent, windowEvent, sizeof(WindowEvent));
	memcpy(&_window->wndcnfg, windowInfo, sizeof(WindowConfigure));

	_window->win32.hInstance = (HINSTANCE)GetModuleHandleA(0);
	_window->System = WINDOWS;
	_window->this_thread.id = (void*)GetCurrentThread();

	DWORD dwStyle, dwExStyle;
	RECT wRect = {0};
	int fullwidth, fullheight, x, y;
	char* windowClassName = (char*)calloc(MAX_CLASS_NAME, 1);

	WNDCLASSEXA wc = { 0 };
	if (GetClassInfoExA(_window->win32.hInstance, windowInfo->appName, &wc) && share == 0) // check if window class is created get the information of the window class
	{
		strcpy(windowClassName, windowInfo->appName);
	}
	else if (share == 0 || ((WindowStruct*)share)->win32.windowClass == 0) // check if window class isn't created
	{
		wc.cbSize = sizeof(WNDCLASSEXA);
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hbrBackground = CreateSolidBrush(0x0);
		wc.hCursor = LoadCursorA(0, MAKEINTRESOURCEA(32512));
		wc.hIcon = LoadIconA(0, MAKEINTRESOURCEA(32517));
		wc.hInstance = (HINSTANCE)GetModuleHandle(0);
		wc.lpfnWndProc = (WNDPROC)windowProc;
		if (windowInfo->appName && windowInfo->appName != "")
		{
			wc.lpszClassName = windowInfo->appName;
		}
		else
		{
			wc.lpszClassName = windowInfo->title;
		}
		
		
		wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wc.lpszMenuName = 0;
		wc.cbClsExtra = 0;
		windowClassName = MAKEINTATOM(RegisterClassExA(&wc));
		if (!windowClassName)
		{
			/*result.errorIndex = 1;
			result.message = "(win32) failed to create window class";
			result.success = false;*/
			return 0;
		}
	}
	else
	{
		windowClassName = ((WindowStruct*)share)->win32.windowClass;
	}

	_window->win32.windowClass = windowClassName;

	dwStyle = WS_VISIBLE | WS_OVERLAPPED;
	dwExStyle = WS_EX_APPWINDOW;
	

	if (windowInfo->flags & WC_FLAG_FULLSCREEN)
	{
		dwExStyle |= WS_EX_TOPMOST;
		dwStyle |= WS_POPUPWINDOW;
		MONITORINFO monitorInfo = { 0 };
		monitorInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfoA(MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTONEAREST), &monitorInfo);
		DEVMODE dm;
		memset(&dm, 0, sizeof(DEVMODE));
		dm.dmSize = sizeof(DEVMODE);
		if (!EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &dm)) 
		{ 
			/*result.errorIndex = 2;
			result.message = "(win32) failed to enum display settings";
			result.success = false;*/
			return 0; 
		}
		dm.dmPelsWidth = monitorInfo.rcMonitor.right;
		dm.dmPelsHeight = monitorInfo.rcMonitor.bottom;
		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
		
		{
			fullwidth = monitorInfo.rcMonitor.right;
			fullheight = monitorInfo.rcMonitor.bottom;
			x = monitorInfo.rcMonitor.left;
			y = monitorInfo.rcMonitor.top;
		}

		_window->wndcnfg.Size.width = fullwidth;
		_window->wndcnfg.Size.height = fullheight;

		LONG res;
		if ((res = ChangeDisplaySettingsA(&dm, CDS_FULLSCREEN)) != DISP_CHANGE_SUCCESSFUL) 
		{
			/*result.user_data = &res;
			result.errorIndex = 3;
			result.message = "(win32) failed to change display settings";
			result.success = false;*/
			return 0; 
		}
	}
	else
	{
		dwStyle |= WS_MINIMIZEBOX;
		if (windowInfo->flags & WC_FLAG_RESIZABLE && windowInfo->flags & WC_FLAG_CAPTION)
		{
			dwStyle |= WS_MAXIMIZEBOX | WS_THICKFRAME;
			dwExStyle |= WS_EX_WINDOWEDGE;
		}

		if (windowInfo->flags & WC_FLAG_CAPTION)
		{
			dwStyle |= WS_CAPTION | WS_SYSMENU;
		}
		else
		{
			dwStyle &= ~WS_OVERLAPPED;
			dwStyle |= WS_POPUPWINDOW ;
			if(windowInfo->flags & WC_FLAG_RESIZABLE)
			{
				dwStyle |= WS_THICKFRAME;
			}
		}
		SetRect(&wRect, 0, 0, windowInfo->Size.width, windowInfo->Size.height);
		AdjustWindowRectEx(&wRect, dwStyle, 0, dwExStyle);
		fullwidth = wRect.right - wRect.left;
		fullheight = wRect.bottom - wRect.top;
		if (windowInfo->Size.flags & WC_SIZE_XCENTER)
		{
			x = (GetSystemMetrics(SM_CXSCREEN) / 2) - (fullwidth / 2);
			_window->wndcnfg.Size.offsetX = x;
		}
		else 
		{
			x = windowInfo->Size.offsetX;
		}
		
		if (windowInfo->Size.flags & WC_SIZE_YCENTER)
		{
			y = (GetSystemMetrics(SM_CYSCREEN) / 2) - (fullheight / 2);
			_window->wndcnfg.Size.offsetY = y;
		}
		else
		{
			y = windowInfo->Size.offsetY;
		}
		
	}

	

	_window->win32.m_hWnd = CreateWindowExA(
		dwExStyle,
		windowClassName,
		windowInfo->title,
		dwStyle,
		x,
		y,
		fullwidth,
		fullheight,
		0,
		0,
		_window->win32.hInstance,
		_window
	);
	if (!_window->win32.m_hWnd)
	{
		/*result.errorIndex = 4;
		result.message = "failed to create window handle";
		result.success = false;*/
		return 0;
	}

	Window* window = (Window*)_window;

	ShowWindow(_window->win32.m_hWnd, 1);
	SetFocus(_window->win32.m_hWnd);

	if (_window->wndcnfg.flags & WC_FLAG_ACCEPTDRAGFILES)
		DragAcceptFiles(_window->win32.m_hWnd, TRUE);

	SendMessage(_window->win32.m_hWnd, WM_FCREATE, _window, 0);

	/*result.errorIndex = 0;
	result.message = "";
	result.success = true;*/
	return window;
}

void getWindowDimensions(Window* window, uint32_t* width, uint32_t* height)
{
	assert("null window" && window);
	*width = ((WindowStruct*)window)->wndcnfg.Size.width;
	*height = ((WindowStruct*)window)->wndcnfg.Size.height;
}

void* getWindowUserDataPointer(Window* window)
{
	assert("null window" && window);
	if(((WindowStruct*)window)->user_data)
	return ((WindowStruct*)window)->user_data;
	else return 0;
}

void setWindowUserDataPointer(Window* window, void* data)
{
	assert("null window" && window);
	((WindowStruct*)window)->user_data = data;
}

HWND getWin32Window(Window* window)
{
	assert("null window" && window);
	return ((WindowStruct*)window)->win32.m_hWnd;
}

HINSTANCE getWin32Instance(Window* window)
{
	assert("null window" && window);
	return ((WindowStruct*)window)->win32.hInstance;
}

bool pollEvent()
{
	MSG message;
	memset(&message, 0, sizeof(MSG));
	if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
		if (message.message == WM_QUIT) return false;

		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return true;
}

static LRESULT windowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	Window* window = (Window*)GetWindowLongPtrA(hWnd, GWLP_USERDATA);
	if (!window)
	{
		if (Msg == WM_NCCREATE)
		{
			const CREATESTRUCT* cs = (const CREATESTRUCT*)lParam;
			SetWindowLongPtrA(hWnd, GWLP_USERDATA, cs->lpCreateParams);
			
			return DefWindowProc(hWnd, Msg, wParam, lParam);
		}
	}

	if (Msg == WM_NCHITTEST)
	{
		if (!((WindowStruct*)window)->wndcnfg.flags & (WC_FLAG_CAPTION | WC_FLAG_FULLSCREEN))
		{
			LRESULT position = DefWindowProc(hWnd, Msg, wParam, lParam);
			return position == HTCLIENT ? HTCAPTION : position;
		}
	}

	switch (Msg)
	{
	case WM_FCREATE:
	{
		if (((WindowStruct*)wParam)->wndEvent.OnUserInitialize) ((WindowStruct*)wParam)->wndEvent.OnUserInitialize((Window*)wParam);
		break;
	}
	case WM_PAINT:
	{
		if (((WindowStruct*)window)->wndEvent.OnUserRender) ((WindowStruct*)window)->wndEvent.OnUserRender(window);
		break;
	}
	case WM_CLOSE:
	{
		if(((WindowStruct*)window)->wndEvent.OnUserQuit) ((WindowStruct*)window)->wndEvent.OnUserQuit(window, (int)wParam);
		break;
	}
	case WM_QUIT:
	{
		DestroyWindow(hWnd);
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		free(window);
		break;
	}
	case WM_KEYDOWN:
	{
		if(((WindowStruct*)window)->wndEvent.OnUserKeyDown) ((WindowStruct*)window)->wndEvent.OnUserKeyDown(window, wParam);
		break;
	}
	case WM_KEYUP:
	{
		if (((WindowStruct*)window)->wndEvent.OnUserKeyUp) ((WindowStruct*)window)->wndEvent.OnUserKeyUp(window, wParam);
		break;
	}
	case WM_MOVE:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		if (((WindowStruct*)window)->wndEvent.OnUserMove) ((WindowStruct*)window)->wndEvent.OnUserMove(window, x, y);
		break;
	}
	case WM_SIZE:
	{
		int width = LOWORD(lParam);
		int height = HIWORD(lParam);
		((WindowStruct*)window)->wndcnfg.Size.width = width;
		((WindowStruct*)window)->wndcnfg.Size.height = height;
		if (((WindowStruct*)window)->wndEvent.OnUserResize) ((WindowStruct*)window)->wndEvent.OnUserResize(window, width, height);
		break;
	}

	case WM_MOUSEMOVE:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		int key = 0;
		switch (wParam)
		{
		case MK_RBUTTON:
		{
			key = MK_RB;
			break;
		}
		case MK_MBUTTON:
		{
			key = MK_MB;
			break;
		}
		case MK_LBUTTON:
		{
			key = MK_LB;
			break;
		}
		case MK_XBUTTON1:
		{
			key = MK_XB1;
			break;
		}
		case MK_XBUTTON2:
		{
			key = MK_XB2;
			break;
		}
		}
		if (((WindowStruct*)window)->wndEvent.OnUserMouseMove) ((WindowStruct*)window)->wndEvent.OnUserMouseMove(window, x, y, key);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) / 120;
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		if (((WindowStruct*)window)->wndEvent.OnUserMouseWheel) ((WindowStruct*)window)->wndEvent.OnUserMouseWheel(window, x, y, wheelDelta);
		break;
	}
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		int key = 0;
		switch (wParam)
		{
		case MK_RBUTTON:
		{
			key = MK_RB;
			break;
		}
		case MK_MBUTTON:
		{
			key = MK_MB;
			break;
		}
		case MK_LBUTTON:
		{
			key = MK_LB;
			break;
		}
		case MK_XBUTTON1:
		{
			key = MK_XB1;
			break;
		}
		case MK_XBUTTON2:
		{
			key = MK_XB2;
			break;
		}
		}
		if (((WindowStruct*)window)->wndEvent.OnUserMouseDoubleClick) ((WindowStruct*)window)->wndEvent.OnUserMouseDoubleClick(window, x, y, key);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		int key = 0;
		switch (wParam)
		{
		case MK_RBUTTON:
		{
			key = MK_RB;
			break;
		}
		case MK_MBUTTON:
		{
			key = MK_MB;
			break;
		}
		case MK_LBUTTON:
		{
			key = MK_LB;
			break;
		}
		case MK_XBUTTON1:
		{
			key = MK_XB1;
			break;
		}
		case MK_XBUTTON2:
		{
			key = MK_XB2;
			break;
		}
		}
		if (((WindowStruct*)window)->wndEvent.OnUserMouseDown) ((WindowStruct*)window)->wndEvent.OnUserMouseDown(window, x, y, key);
		break;
	}
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		int key = 0;
		switch (wParam)
		{
		case MK_RBUTTON:
		{
			key = MK_RB;
			break;
		}
		case MK_MBUTTON:
		{
			key = MK_MB;
			break;
		}
		case MK_LBUTTON:
		{
			key = MK_LB;
			break;
		}
		case MK_XBUTTON1:
		{
			key = MK_XB1;
			break;
		}
		case MK_XBUTTON2:
		{
			key = MK_XB2;
			break;
		}
		}
		if (((WindowStruct*)window)->wndEvent.OnUserMouseUp) ((WindowStruct*)window)->wndEvent.OnUserMouseUp(window, x, y, key);
		break;
	}
	case WM_DROPFILES:
	{
		HDROP hDrop = (HDROP)wParam;
		char **filePaths;

		UINT fileCount = DragQueryFileA(hDrop, 0xFFFFFFFF, NULL, 0);

		filePaths = calloc(fileCount, sizeof(char*));
		for (UINT i = 0; i < fileCount; i++) {
			char* filePath;
			filePath = calloc(MAX_PATH, 1);
			DragQueryFileA(hDrop, i, filePath, MAX_PATH);
			if(!filePaths[i])
				filePaths[i] = filePath;
		}

		if (((WindowStruct*)window)->wndEvent.OnUserDragFile) ((WindowStruct*)window)->wndEvent.OnUserDragFile(window, filePaths, fileCount);

		for (UINT i = 0; i < fileCount; i++)
		{
			free(filePaths[i]);
		}

		DragFinish(hDrop); // Free resources
		break;
	}
	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}
