
/* 
* Copyright (c) 2024-2025 Ki(y)arash Salehi
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
* documentation files (the "Software"), to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
* to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of 
* the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
* THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#define USE_WINDOW_STRUCT_

#include "./defWindow.h"

#ifdef _WIN32

#include <shellapi.h>

#define WM_FCREATE (WM_USER + 1)

#define MAX_CLASS_NAME 512

static LRESULT windowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

CFWindow CFCreateWindow(
	const WindowConfigure* 				windowInfo, 
	const WindowEventHandle* 					windowEvent, 
	CFWindow 							share)
{
	WindowStruct* _window = calloc(1, sizeof(WindowStruct));

	if(windowEvent)
		memcpy(&_window->wndEvent, windowEvent, sizeof(WindowEventHandle));
	memcpy(&_window->wndcnfg, windowInfo, sizeof(WindowConfigure));

	_window->win32.hInstance = (HINSTANCE)GetModuleHandleA(0);
	_window->System = WINDOWS;
	_window->this_thread.id = (void*)GetCurrentThread();

	DWORD dwStyle, dwExStyle;
	RECT wRect = {0};
	int fullwidth, fullheight, x, y;
	char* windowClassName = 0;

	WNDCLASSEXA wc = { 0 };
	if (GetClassInfoExA(_window->win32.hInstance, windowInfo->appName, &wc) && share == 0) // check if window class is created get the information of the window class
	{
		windowClassName = windowInfo->appName;
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
		
		
		wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
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

	CFWindow window = (CFWindow)_window;

	ShowWindow(_window->win32.m_hWnd, 1);
	SetFocus(_window->win32.m_hWnd);

	if (_window->wndcnfg.flags & WC_FLAG_ACCEPTDRAGFILES)
		DragAcceptFiles(_window->win32.m_hWnd, TRUE);

	SendMessageA(_window->win32.m_hWnd, WM_FCREATE, _window, 0);

	/*result.errorIndex = 0;
	result.message = "";
	result.success = true;*/
	return window;
}

void getWindowDimensions(CFWindow window, uint32_t* width, uint32_t* height)
{
	assert("null window" && window);
	*width = ((WindowStruct*)window)->wndcnfg.Size.width;
	*height = ((WindowStruct*)window)->wndcnfg.Size.height;
}

void* getWindowUserDataPointer(CFWindow window)
{
	assert("null window" && window);
	if(((WindowStruct*)window)->user_data)
	return ((WindowStruct*)window)->user_data;
	else return 0;
}

void setWindowUserDataPointer(CFWindow window, void* data)
{
	assert("null window" && window);
	((WindowStruct*)window)->user_data = data;
}

void setUserEventCallback(WindowEventHandle* event, CFWindow window)
{
	((WindowStruct*)window)->wndEvent = *event;
}

WindowEventHandle* getWindowEventHandle(CFWindow window)
{
	return &((WindowStruct*)window)->wndEvent;
}

void* getWin32Window(CFWindow window)
{
	assert("null window" && window);
	return (void*)((WindowStruct*)window)->win32.m_hWnd;
}

void* getWin32Instance(CFWindow window)
{
	assert("null window" && window);
	return (void*)((WindowStruct*)window)->win32.hInstance;
}

void setWindowTitle(CFWindow window, const char* const title)
{
	assert("null window" && window);
	if (!title) return;
	if (title != ((WindowStruct*)window)->wndcnfg.title)
	{
		((WindowStruct*)window)->wndcnfg.title = title;
	}

	SetWindowTextA(((WindowStruct*)window)->win32.m_hWnd, title);
}

static void _centerWindowPos(CFWindow window)
{
	MONITORINFO monitorInfo = { 0 };
	monitorInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfoA(MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTONEAREST), &monitorInfo);
	int x = (monitorInfo.rcMonitor.right - ((WindowStruct*)window)->wndcnfg.Size.width) / 2;
	int y = (monitorInfo.rcMonitor.bottom - ((WindowStruct*)window)->wndcnfg.Size.height) / 2;
	SetWindowPos(((WindowStruct*)window)->win32.m_hWnd, 0, x, y, 0, 0, SWP_NOSIZE);
	((WindowStruct*)window)->wndcnfg.Size.offsetX = x;
	((WindowStruct*)window)->wndcnfg.Size.offsetX = y;
}

void setWindowSize(CFWindow window, int width, int height)
{
	assert("null window" && window);
	((WindowStruct*)window)->wndcnfg.Size.width = width;
	((WindowStruct*)window)->wndcnfg.Size.height = height;
	SetWindowPos(((WindowStruct*)window)->win32.m_hWnd, 0, 0, 0, width, height, SWP_NOMOVE | SWP_DRAWFRAME);
}



//static MSG message;
static Event event;

bool pollEvent()
{
	MSG message;
	memset(&message, 0, sizeof(MSG));
	if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
		if (message.message == WM_QUIT) return false;

		TranslateMessage(&message);
		DispatchMessage(&message);

		if (message.message == WM_SIZE) event.type = ONUSERRESIZE;
	}
	else
	{
		memset(&event.event, 0, sizeof(event.event));
		event.type = 0;
	}

	return true;
}

Event* CFGetWindowEvent()
{
	return &event;
}

static LRESULT windowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CFWindow window = (CFWindow)GetWindowLongPtrA(hWnd, GWLP_USERDATA);
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

	if(Msg != WM_QUIT)
	{
		event.window = window;
		event.handler = &((WindowStruct*)window)->wndEvent;
	}

	switch (Msg)
	{
	case WM_FCREATE:
	{
		event.type = ONUSERINITIALIZE;
		event.event.ONUSERINITIALIZE.window = window;
		if (((WindowStruct*)wParam)->wndEvent.OnUserInitialize) ((WindowStruct*)wParam)->wndEvent.OnUserInitialize((CFWindow*)wParam);
		break;
	}
	case WM_PAINT:
	{
		event.type = ONUSERRENDER;
		event.event.ONUSERRENDER.window = window;
		if (((WindowStruct*)window)->wndEvent.OnUserRender) ((WindowStruct*)window)->wndEvent.OnUserRender(window);
		break;
	}
	case WM_CLOSE:
	{
		event.type = ONUSERQUIT;
		event.event.ONUSERQUIT.quitCode = (int)wParam;
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
		event.type = ONUSERKEYDOWN;
		event.event.ONUSERKEYDOWN.key = wParam;
		if(((WindowStruct*)window)->wndEvent.OnUserKeyDown) ((WindowStruct*)window)->wndEvent.OnUserKeyDown(window, wParam);
		break;
	}
	case WM_KEYUP:
	{
		event.type = ONUSERKEYUP;
		event.event.ONUSERKEYUP.key = wParam;
		if (((WindowStruct*)window)->wndEvent.OnUserKeyUp) ((WindowStruct*)window)->wndEvent.OnUserKeyUp(window, wParam);
		break;
	}
	case WM_MOVE:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		event.type = ONUSERMOVE;
		event.event.ONUSERMOVE.x = x;
		event.event.ONUSERMOVE.y = y;
		if (((WindowStruct*)window)->wndEvent.OnUserMove) ((WindowStruct*)window)->wndEvent.OnUserMove(window, x, y);
		break;
	}
	case WM_SIZE:
	{
		RECT rect = { 0 };
		GetClientRect(hWnd, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		((WindowStruct*)window)->wndcnfg.Size.width = width;
		((WindowStruct*)window)->wndcnfg.Size.height = height;
		event.type = ONUSERRESIZE;
		*(uint32_t*)&event.event.ONUSERRESIZE = *(uint32_t*)&((WindowStruct*)window)->wndcnfg.Size;
		((uint32_t*)&event.event.ONUSERRESIZE)[1] = ((uint32_t*)&((WindowStruct*)window)->wndcnfg.Size)[1];
		/* asm:
		* movl $_event, %esp
		* movl 32(%window), 12(%esp)
		* movl 36(%window), 16(%esp)
		*/
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
		event.type = ONUSERMOUSEMOVE;
		event.event.ONUSERMOUSEMOVE.x = x;
		event.event.ONUSERMOUSEMOVE.y = y;
		event.event.ONUSERMOUSEMOVE.key = key;
		if (((WindowStruct*)window)->wndEvent.OnUserMouseMove) ((WindowStruct*)window)->wndEvent.OnUserMouseMove(window, x, y, key);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) / 120;
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		event.type = ONUSERMOUSEWHEEL;
		event.event.ONUSERMOUSEWHEEL.x = x;
		event.event.ONUSERMOUSEWHEEL.y = y;
		event.event.ONUSERMOUSEWHEEL.dir = wheelDelta;
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
		event.type = ONUSERMOUSEDOUBLECLK;
		event.event.ONUSERMOUSEDOUBLECLK.x = x;
		event.event.ONUSERMOUSEDOUBLECLK.y = y;
		event.event.ONUSERMOUSEDOUBLECLK.keyIndex = key;
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
		event.type = ONUSERMOUSEDOWN;
		event.event.ONUSERMOUSEDOWN.x = x;
		event.event.ONUSERMOUSEDOWN.y = y;
		event.event.ONUSERMOUSEDOWN.keyIndex = key;
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
		event.type = ONUSERMOUSEUP;
		event.event.ONUSERMOUSEUP.x = x;
		event.event.ONUSERMOUSEUP.y = y;
		event.event.ONUSERMOUSEUP.keyIndex = key;
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

		event.type = ONUSERDRAGFILE;
		event.event.ONUSERDRAGFILE.files = filePaths;
		event.event.ONUSERDRAGFILE.count = fileCount;

		if (((WindowStruct*)window)->wndEvent.OnUserDragFile) ((WindowStruct*)window)->wndEvent.OnUserDragFile(window, filePaths, fileCount);

		for (UINT i = 0; i < fileCount; i++)
		{
			free(filePaths[i]);
		}
		free(filePaths);

		DragFinish(hDrop); // Free resources
		break;
	}
	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

#endif 