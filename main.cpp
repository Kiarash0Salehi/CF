

#define USE_WINDOW_STRUCT_

#include "win32_thread.h"
#include "WindowEvent.h"
#include "Image.h"
#include "win32_Window.h"

#include <stdio.h>
#include <thread>
#include <functional>

static Window Mainwindow;
bool running = 1;

void Init(Window* window)
{
	printf("init\n");
	//TerminateThread(((WindowStruct*)window)->this_thread.id, 0);
	//ExitThread(0);
	
	int x = 10;
	setWindowUserDataPointer(window, &x);
}
void Quit(Window* window, int code)
{	
	uint32_t x = 0, y = 0;
	for(int i = 0; i < 6; i++)
	getWindowDimensions(window, &x, &y);

	printf("width : %d, height : %d\n", x, y);
	printf("quit code : %d\n", code);
	running = 0;
}
void keydown(Window* window, uintptr_t key)
{
	if (key == 'Q') exit(1);
	int c = *(int*)getWindowUserDataPointer(window);
	printf("user data : %d\n", c);
}
void keyup(Window* window, uintptr_t key)
{
	
}

int VGMain
{
	
	WindowEvent eventHandle = {};
	eventHandle.OnUserInitialize = Init;
	eventHandle.OnUserQuit = Quit;
	eventHandle.OnUserKeyDown = (ONUSERKEYDOWNCALLBACK)keydown;
	eventHandle.OnUserKeyUp = keyup;

	WindowConfigure winInfo = {};
	Mainwindow = 0;

	winInfo.title = "test";
	winInfo.appName = "t";
	winInfo.Size.height = 500;
	winInfo.Size.width = 500;
	winInfo.Size.flags = WC_SIZE_XCENTER | WC_SIZE_YCENTER;
	winInfo.flags = WC_FLAG_CAPTION | WC_FLAG_RESIZABLE;


	int result;
	if ((result = GFCreateWindow(winInfo, &Mainwindow, &eventHandle))) { printf("error : %d\n", result); __debugbreak(); }

	while (running)
	{
		if(!(result = doEvent(&Mainwindow))) printf("event error : %d", result);
	}
	
	return 0;
}