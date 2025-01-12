#pragma once
#include "defWindow.h"

#ifndef _WIN32_WINDOW_H_
#define _WIN32_WINDOW_H_

#ifdef __cplusplus
extern "C"
{
#endif

int guiCreateWindow(WindowConfigure windowInfo, Window* window, WindowEvent windowEvent);
int MainLoop(const Window* window);

#ifdef __cplusplus
}
#endif

#endif // _WIN32_WINDOW_H_
