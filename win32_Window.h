#pragma once
#include "defWindow.h"

#ifdef __cplusplus
extern "C"
{
#endif

int guiCreateWindow(WindowInfo windowInfo, Window* window, WindowEvent windowEvent);
int MainLoop(Window* window);

#ifdef __cplusplus
}
#endif
