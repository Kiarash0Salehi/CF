#include "defWindow.h"

static void OnUserMouseMove(Window* window, int x, int y) {}
static void OnUserMouseWheel(Window* window, float pos) {}
static void OnUserMouseDown(Window* window, unsigned char key) {}
static void OnUserKeyDown(Window* window, unsigned char key) {}
static void OnUserKeyUp(Window* window, unsigned char key) {}
static void OnUserResize(Window* window, int width, int height) {}
static void OnUserInitialize(Window* window) {}
static void OnUserRender(Window* window) {}
static void OnUserMove(Window* window, int x, int y) {}
static void OnUserQuit(Window* window, int code) {}

WindowEvent* DefWindowEvent()
{
	WindowEvent windowEvent;
	windowEvent.OnUserInitialize = OnUserInitialize;
	windowEvent.OnUserKeyDown = OnUserKeyDown;
	windowEvent.OnUserKeyUp = OnUserKeyUp;
	windowEvent.OnUserMouseDown = OnUserMouseDown;
	windowEvent.OnUserMouseMove = OnUserMouseMove;
	windowEvent.OnUserMouseWheel = OnUserMouseWheel;
	windowEvent.OnUserMove = OnUserMove;
	windowEvent.OnUserRender = OnUserRender;
	windowEvent.OnUserQuit = OnUserQuit;
	windowEvent.OnUserResize = OnUserResize;
	return &windowEvent;
}