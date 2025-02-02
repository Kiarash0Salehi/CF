#define USE_WINDOW_STRUCT_

#include "defWindow.h"

#include <stdio.h>
#include "win32_Window.h"

#define WM_NOCAPTIONDRAG (WM_USER + 1)

static WindowStruct windowStruct(Window window)
{
	return *(WindowStruct*)&window;
}

#define name(o) #o

static WindowEvent windowEvent;
static LRESULT windowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

int GFCreateWindow(WindowConfigure windowInfo, Window* window_handle, WindowEvent* _windowEvent)
{
	memcpy(&windowEvent, _windowEvent, sizeof(WindowEvent));
	WindowStruct window = *(WindowStruct*)(uintptr_t)window_handle;
	if (*window_handle == 0) 
	{ 
		memset(&window, 0, 8);
		memset(&window.win32, 0, sizeof(window.win32));
	}
	
	

	DWORD dwStyle, dwExStyle;
	RECT wRect = {0};
	int fullwidth, fullheight, x, y;
	if (window.win32.windowClass == 0) // cheak if window class isn't created
	{
		WNDCLASSA wc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hbrBackground = 0;
		wc.hCursor = LoadCursorA(0, MAKEINTRESOURCEA(32512));
		wc.hIcon = LoadIconA(0, MAKEINTRESOURCEA(32517));
		wc.hInstance = window.win32.hInstance;
		wc.lpfnWndProc = (WNDPROC)windowProc;
		if (windowInfo.appName && windowInfo.appName != "")
		{
			wc.lpszClassName = windowInfo.appName;
			window.win32.windowClass = windowInfo.appName;
		}
		else
		{
			wc.lpszClassName = windowInfo.title;
			window.win32.windowClass = windowInfo.title;
		}
		
		
		wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wc.lpszMenuName = 0;
		wc.cbClsExtra = 0;
		if(!RegisterClassA(&wc)) return -1;
	}

	dwStyle = WS_VISIBLE | WS_OVERLAPPED;
	dwExStyle = WS_EX_APPWINDOW;
	

	if (windowInfo.flags & WC_FLAG_FULLSCREEN)
	{
		long result;
		dwExStyle |= WS_EX_TOPMOST;
		dwStyle |= WS_POPUPWINDOW;
		MONITORINFO monitorInfo = { 0 };
		monitorInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfoA(MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTONEAREST), &monitorInfo);
		DEVMODE dm;
		memset(&dm, 0, sizeof(DEVMODE));
		dm.dmSize = sizeof(DEVMODE);
		if (!EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &dm)) { MessageBoxA(0, "faild to enum dispaly sttings", "Error", MB_OK); return 0; }
		dm.dmPelsWidth = monitorInfo.rcMonitor.right;
		dm.dmPelsHeight = monitorInfo.rcMonitor.bottom;
		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
		
		fullwidth = monitorInfo.rcMonitor.right;
		fullheight = monitorInfo.rcMonitor.bottom;
		windowInfo.Size.width = fullwidth;
		windowInfo.Size.height = fullheight;
		x = monitorInfo.rcMonitor.left;
		y = monitorInfo.rcMonitor.top;
		
		if ((result = ChangeDisplaySettings(&dm, CDS_FULLSCREEN)) != DISP_CHANGE_SUCCESSFUL) { MessageBoxA(0, "failed to change dispaly sttings", "Error", MB_OK); return result; }
	}
	else
	{
		dwStyle |= WS_MINIMIZEBOX;
		if (windowInfo.flags & WC_FLAG_RESIZABLE && windowInfo.flags & WC_FLAG_CAPTION)
		{
			dwStyle |= WS_MAXIMIZEBOX | WS_THICKFRAME;
			dwExStyle |= WS_EX_WINDOWEDGE;
		}

		if (windowInfo.flags & WC_FLAG_CAPTION)
		{
			dwStyle |= WS_CAPTION | WS_SYSMENU;
		}
		else
		{
			dwStyle &= ~WS_OVERLAPPED;
			dwStyle |= WS_POPUPWINDOW ;
			if(windowInfo.flags & WC_FLAG_RESIZABLE)
			{
				dwStyle |= WS_THICKFRAME;
			}
		}
		SetRect(&wRect, 0, 0, windowInfo.Size.width, windowInfo.Size.height);
		AdjustWindowRectEx(&wRect, dwStyle, 0, dwExStyle);
		fullwidth = wRect.right - wRect.left;
		fullheight = wRect.bottom - wRect.top;
		if (windowInfo.Size.flags & WC_SIZE_XCENTER)
		{
			x = (GetSystemMetrics(SM_CXSCREEN) / 2) - (fullwidth / 2);
			windowInfo.Size.offsetX = x;
		}
		else 
		{
			x = windowInfo.Size.offsetX;
		}
		
		if (windowInfo.Size.flags & WC_SIZE_YCENTER)
		{
			y = (GetSystemMetrics(SM_CYSCREEN) / 2) - (fullheight / 2);
			windowInfo.Size.offsetY = y;
		}
		else
		{
			y = windowInfo.Size.offsetY;
		}
		
	}

	

	window.win32.m_hWnd = CreateWindowExA(
		dwExStyle,
		window.win32.windowClass,
		windowInfo.title,
		dwStyle,
		x,
		y,
		fullwidth,
		fullheight,
		window.win32.parent == 0 ? 0 : window.win32.parent,
		0,
		window.win32.hInstance,
		&window_handle
	);
	if (!window.win32.m_hWnd) return -1;

	printf("window created! \n");
	
	bool child = false;
	if (!window.win32.parent) window.win32.parent = window.win32.m_hWnd; else child = true;

	ShowWindow(window.win32.m_hWnd, 1);
	SetFocus(window.win32.m_hWnd);
	
	window.wndcnfg = windowInfo;
	window.System = WINDOWS;
	window.this_thread.id = (void*)GetCurrentThread();
	window.win32.hInstance = (HINSTANCE)GetModuleHandleA(0);
	
	int data = 0;
	window.user_data = &data;

	if (!child)
	{
		*(WindowStruct*)window_handle = *(WindowStruct*)&window;
	}
	else
	{
		
	}
	return 0;
}

void getWindowDimensions(Window* window, uint32_t* width, uint32_t* height)
{
	if (!window) return;
	*width = ((WindowStruct*)window)->wndcnfg.Size.width;
	*height = ((WindowStruct*)window)->wndcnfg.Size.height;
}

void* getWindowUserDataPointer(Window* window)
{
	return ((WindowStruct*)window)->user_data;
}

void setWindowUserDataPointer(Window* window, void* data)
{
	((WindowStruct*)window)->user_data = data;
}

HWND getWin32Window(Window* window)
{
	return ((WindowStruct*)window)->win32.m_hWnd;
}

HINSTANCE getWin32Instance(Window* window)
{
	return ((WindowStruct*)window)->win32.hInstance;
}

bool doEvent(Window* window)
{
	BOOL result;
	if (result = PeekMessage(&((WindowStruct*)window)->win32.message, getWin32Window(window), 0, 0, PM_REMOVE))
	{
		if (result == -1) return false;
		TranslateMessage(&((WindowStruct*)window)->win32.message);
		DispatchMessage(&((WindowStruct*)window)->win32.message);
	}
	return true;
}

// TO-DO : finish this windowProc function and write the rest of it
static LRESULT windowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_CREATE:
	{
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)((LPCREATESTRUCT)lParam)->lpCreateParams);
		if(windowEvent.OnUserInitialize != 0) windowEvent.OnUserInitialize((Window*)GetWindowLongPtrA(hWnd, GWLP_USERDATA));
		return 0;
	}
	case WM_PAINT:
	{
		if(windowEvent.OnUserRender != 0) windowEvent.OnUserRender((Window**)GetWindowLongPtrA(hWnd, GWLP_USERDATA));
		return 0;
	}
	case WM_CLOSE:
	{
		if(windowEvent.OnUserQuit != 0) windowEvent.OnUserQuit((Window**)GetWindowLongPtrA(hWnd, GWLP_USERDATA), (int)wParam);
		DestroyWindow(hWnd);
		return 0;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_KEYDOWN:
	{
		if(windowEvent.OnUserKeyDown != 0) windowEvent.OnUserKeyDown((Window**)GetWindowLongPtrA(hWnd, GWLP_USERDATA), wParam);
		return 0;
	}
	case WM_KEYUP:
	{
		if (windowEvent.OnUserKeyUp != 0) windowEvent.OnUserKeyUp((Window**)GetWindowLongPtrA(hWnd, GWLP_USERDATA), wParam);
		return 0;
	}
	case WM_NCHITTEST:
	{
		if (((WindowStruct*)(Window**)GetWindowLongPtrA(hWnd, GWLP_USERDATA))->wndcnfg.flags & ~(WC_FLAG_CAPTION | WC_FLAG_FULLSCREEN))
		{
			LRESULT position = DefWindowProc(hWnd, Msg, wParam, lParam);
			return position == HTCLIENT ? HTCAPTION : position;
		}
		else
			return 0;
	}
	}
	return DefWindowProcA(hWnd, Msg, wParam, lParam);
}
