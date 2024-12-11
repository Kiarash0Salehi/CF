#include "WindowEngine.h"

void OnUserMouseMove(int x, int y) {}
void OnUserMouseWheel(float pos) {}
void OnUserMouseDown(unsigned char right) {}
void OnUserKeyDown(unsigned char key) {}
void OnUserKeyUp(unsigned char key) {}
void OnUserResize(int width, int height) {}
void OnUserInitialize() {}
void OnUserPaint() {}
void OnUserMove(int x, int y) {}
void OnUserQuit(int code) {}

WindowEvent EmptyWindowEvent()
{
	WindowEvent windowEvent;
	windowEvent.OnUserInitialize = OnUserInitialize;
	windowEvent.OnUserKeyDown = OnUserKeyDown;
	windowEvent.OnUserKeyUp = OnUserKeyUp;
	windowEvent.OnUserMouseDown = OnUserMouseDown;
	windowEvent.OnUserMouseMove = OnUserMouseMove;
	windowEvent.OnUserMouseWheel = OnUserMouseWheel;
	windowEvent.OnUserMove = OnUserMove;
	windowEvent.OnUserPaint = OnUserPaint;
	windowEvent.OnUserQuit = OnUserQuit;
	windowEvent.OnUserResize = OnUserResize;
	return windowEvent;
}