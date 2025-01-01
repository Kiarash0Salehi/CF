#include "defWindow.h"

#include <stdio.h>

static WindowEvent windowEvent;
static LRESULT windowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
static bool caption;

int guiCreateWindow(WindowInfo windowInfo, Window* window, WindowEvent _windowEvent)
{
	memset(window, 0, sizeof(Window));
	memcpy(&windowEvent, &_windowEvent, sizeof(WindowEvent));
	DWORD dwStyle, dwExStyle;
	RECT wRect = {0};
	int fullwidth, fullheight, x, y;
	if (!window->win32.windowClass) // cheak if window class isn't created
	{
		WNDCLASSA wc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hbrBackground = 0;
		wc.hCursor = LoadCursorA(0, MAKEINTRESOURCEA(32512));
		wc.hIcon = LoadIconA(0, MAKEINTRESOURCEA(32517));
		wc.hInstance = GetModuleHandleA(0);
		wc.lpfnWndProc = (WNDPROC)windowProc;
		if (windowInfo.name)
		{
			wc.lpszClassName = windowInfo.name;
			window->win32.windowClass = windowInfo.name;
		}
		else
		{
			wc.lpszClassName = windowInfo.title;
			window->win32.windowClass = windowInfo.title;
		}
		
		
		wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wc.lpszMenuName = 0;
		wc.cbClsExtra = 0;
		RegisterClassA(&wc);
	}

	dwStyle = WS_VISIBLE | WS_OVERLAPPED;
	dwExStyle = WS_EX_APPWINDOW;

	window->width = windowInfo.width;
	window->height = windowInfo.height;
	

	if (windowInfo.fullscreen)
	{
		dwExStyle |= WS_EX_TOPMOST;
		dwStyle |= WS_POPUPWINDOW;
		MONITORINFO monitorInfo = { 0 };
		monitorInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfoA(MonitorFromWindow(GetDesktopWindow(),MONITOR_DEFAULTTONEAREST), &monitorInfo);
		DEVMODE dm;
		memset(&dm, 0, sizeof(DEVMODE));
		dm.dmSize = sizeof(DEVMODE);
		if (!EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &dm)) { MessageBoxA(0, "faild to enum dispaly sttings", "Error", MB_OK); return 0; }
		dm.dmPelsWidth = monitorInfo.rcMonitor.right;
		dm.dmPelsHeight = monitorInfo.rcMonitor.bottom;
		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
		
		fullwidth = monitorInfo.rcMonitor.right;
		fullheight = monitorInfo.rcMonitor.bottom;
		x = monitorInfo.rcMonitor.left;
		y = monitorInfo.rcMonitor.top;
		long result;
		if ((result = ChangeDisplaySettings(&dm, CDS_FULLSCREEN)) != DISP_CHANGE_SUCCESSFUL) { MessageBoxA(0, "failed to change dispaly sttings", "Error", MB_OK); return result; }
	}
	else
	{
		dwStyle |= WS_MINIMIZEBOX;
		if (windowInfo.resizable)
		{
			dwStyle |= WS_MAXIMIZEBOX | WS_THICKFRAME;
			dwExStyle |= WS_EX_WINDOWEDGE;
		}

		if (windowInfo.caption)
		{
			dwStyle |= WS_CAPTION | WS_SYSMENU;
			caption = true;
		}
		else
		{
			dwStyle &= WS_OVERLAPPED;
			dwStyle |= WS_POPUPWINDOW ;
			if(windowInfo.resizable)
			{
				dwStyle |= WS_THICKFRAME;
			}
			caption = false;
		}
		SetRect(&wRect, 0, 0, windowInfo.width, windowInfo.height);
		AdjustWindowRectEx(&wRect, dwStyle, 0, dwExStyle);
		fullwidth = wRect.right - wRect.left;
		fullheight = wRect.bottom - wRect.top;
		x = (GetSystemMetrics(SM_CXSCREEN) / 2) / (fullwidth / 2);
		y = (GetSystemMetrics(SM_CYSCREEN) / 2) / (fullheight / 2);
	}

	HWND parent = 0;
	if(window->win32.m_hWnd) parent = GetParent(window->win32.m_hWnd);

	window->win32.m_hWnd = CreateWindowExA(
		dwExStyle,
		window->win32.windowClass,
		windowInfo.title,
		dwStyle,
		x,
		y,
		fullwidth,
		fullheight,
		parent,
		0,
		(HINSTANCE)GetModuleHandleA(0),
		0
	);

	ShowWindow(window->win32.m_hWnd, 1);
	SetFocus(window->win32.m_hWnd);

	return 0;
}

int MainLoop(const Window* window)
{
	MSG message;
	memset(&message, 0, sizeof(MSG));
	uint8_t running = 1;
	while (running)
	{
		if (PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
			{
				running = 0;
				break;
			}
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
	return 0;
}

// TO-DO : finish this windowProc function and write the rest of it
LRESULT windowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_CREATE:
	{
		windowEvent.OnUserInitialize();
		return 0;
	}
	case WM_PAINT:
	{
		windowEvent.OnUserPaint();
		return 0;
	}
	case WM_CLOSE:
	case WM_QUIT:
	{
		windowEvent.OnUserQuit((int)wParam);
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
		windowEvent.OnUserKeyDown((unsigned char)wParam);
		return 0;
	}
	case WM_NCHITTEST:
	{
		if(!caption)
		{
    		LRESULT position = DefWindowProc(hWnd, Msg, wParam, lParam);
    		return position == HTCLIENT ? HTCAPTION : position;
		}
		else return 0;
	}
	}
	return DefWindowProcA(hWnd, Msg, wParam, lParam);
}
