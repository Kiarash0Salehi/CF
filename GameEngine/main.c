
#include <stdio.h>

#include "win32_Window.h"
#include "WindowEvent.h"
#include "Image.h"

void Init()
{
	printf("init\n");
}
void Quit(int code)
{
	printf("quit code : %d\n", code);
}
void keydown(unsigned char key)
{
	if (key == 'Q') exit(1);
}

int main() //WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	WindowInfo winInfo;
	Window window;
	memset(&winInfo, 0, sizeof(WindowInfo));
	memset(&window, 0, sizeof(Window));
	winInfo.title = "test";
	winInfo.caption = false;
	winInfo.child = false;
	winInfo.fullscreen = false;
	winInfo.name = "testClass";
	winInfo.height = 300;
	winInfo.width = 300;
	winInfo.resizable = true;

	WindowEvent eventHandle = EmptyWindowEvent();
	eventHandle.OnUserInitialize = Init;
	eventHandle.OnUserQuit = Quit;
	eventHandle.OnUserKeyDown = keydown;

	Image image;
	image = readBMPFromFile("C:\\Users\\Kiarash\\Desktop\\BMP2.bmp");

	printf("width : %d   height : %d   size : %d \n", image.width, image.height, sizeof((void*)image.memory));

	int result;
	if ((result = eCreateWindow(winInfo, &window, eventHandle))) printf("error : %d\n", result);
	MainLoop(&window);
	//free(image);
	return 0;
}