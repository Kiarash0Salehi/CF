
/* 
* Copyright (c) 2025 Ki(y)arash Salehi
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

#include <Window.h>

void resize(Window* window, int width, int height)
{
    int* ptr = getWindowUserDataPointer(window);
    if(ptr)
    printf("width : %d height : %d window pointer : %p user data : %d\n", width, height, window, *ptr);
	setWindowTitle(window, "resized");
}

#ifdef DIST
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main()
#endif
{
    // initize the window configure and window event

    // window configure is a struct that needed for the window creation
    WindowConfigure windcng     = {0};

    // window event is a list of function callback that will make window event handling posible
    // for example I set the resize callback (OnUserResize) to the function that I wrote. 
    WindowEvent windowEvent     = {0};
    windowEvent.OnUserResize          = resize;

    // now setting the configurtion
    windcng.appName     = "test";
    windcng.flags       = WC_FLAG_CAPTION | WC_FLAG_RESIZABLE;
    windcng.title       = "test";
    windcng.Size.flags  = WC_SIZE_XCENTER | WC_SIZE_YCENTER;
    windcng.Size.width  = 500;
    windcng.Size.height = 500;

    //Window* GFCreateWindow(
	//  const WindowConfigure* 				windowInfo, 
	//  const WindowEvent* 					windowEvent, 
	//  Window* 							share)

    // @return window pointer
    //
    // @param 
    // const WindowConfigure* :
    // constant pointer to the windowConfigure 
    //
    // const WindowEvent* :
    // constant poiner to the window event callbacks 
    //
    // Window* :
    // share/parent window

    Window *window = GFCreateWindow(&windcng, &windowEvent, 0);


    // set user data pointer
    int i = 255;
    setWindowUserDataPointer(window, &i);

    // main loop
    while(1)
    {
        if (!pollEvent()) return 0;
        printf("loop\n");
    }

    return 0;
}