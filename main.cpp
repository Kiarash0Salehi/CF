


#include "win32_Window.h"
#include "win32_thread.h"
#include "WindowEvent.h"
#include "Image.h"

#include <stdio.h>
#include <thread>
#include <functional>

bool running = 1;

void Init()
{
	printf("init\n");
}
void Quit(int code)
{
	printf("quit code : %d\n", code);
	running = 0;
}
void keydown(unsigned char key)
{
	if (key == 'Q') exit(1);
	OutputDebugStringA((const char*) & key);
}

void create(void* we)
{
	WindowInfo winInfo = {};
	Window Mainwindow;
	Window Childwindow;
	WindowEvent* windowEvent = static_cast<WindowEvent*>(we);

	winInfo.title = "test";
	winInfo.caption = true;
	winInfo.child = false;
	winInfo.fullscreen = false;
	winInfo.name = "testClass";
	winInfo.height = 600;
	winInfo.width = 800;
	winInfo.resizable = false;


	int result;
	if ((result = guiCreateWindow(winInfo, &Mainwindow, *windowEvent))) printf("error : %d\n", result);
	winInfo.title = "child";
	winInfo.caption = false;
	winInfo.child = true;
	winInfo.fullscreen = false;
	winInfo.name = "testClass";
	winInfo.height = 300;
	winInfo.width = 600;
	winInfo.resizable = true;
	if ((result = guiCreateWindow(winInfo, &Childwindow, *windowEvent))) printf("error : %d\n", result);
	printf("create Thread\n");
	MainLoop(&Mainwindow);
};


int VGMain
{
	
	WindowEvent eventHandle = EmptyWindowEvent();
	eventHandle.OnUserInitialize = Init;
	eventHandle.OnUserQuit = Quit;
	eventHandle.OnUserKeyDown = keydown;

	thread _Thread;
	initThread(&_Thread, (THREADCALLBACK)create, (void*)&eventHandle);

	while(running); puts("main thread output\n");
	//deleteThread(&_Thread);
	return 0;
}