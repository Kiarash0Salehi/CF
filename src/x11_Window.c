
#define USE_WINDOW_STRUCT_
#include "./defWindow.h"   

#ifdef __linux__

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <limits.h>

typedef struct {
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long input_mode;
    unsigned long status;
} MotifHints;

#define MWM_HINTS_DECORATIONS (1L << 1)

static Display* display;
static Event event;
static XEvent xevent;
static XContext windowContext;
static uint32_t windowCount;
static struct {
    int x, y;
    Time time;
    int button;
    bool double_click : 1;
} last_mouse_click;

#define MAX_DOUBLECLICK_THERSHOLD 900
#define MIN_DOUBLECLICK_THERSHOLD 200


CFWindow CFCreateWindow(
    const WindowConfigure* windowInfo,
    const WindowEventHandle* windowEvent,
    CFWindow 				    share)
{
    if (!windowContext)
    {
        windowContext = XUniqueContext();
    }
    WindowStruct* _window = calloc(1, sizeof(WindowStruct));

    _window->System = LINUX;

    if(windowEvent)
        memcpy(&_window->wndEvent, windowEvent, sizeof(WindowEventHandle));
    if (windowInfo)
        memcpy(&_window->wndcnfg, windowInfo, sizeof(WindowConfigure));

    if(!display)
    {
        display = XOpenDisplay(0);
        if (display == NULL) {
            fprintf(stderr, "Unable to open X display\n");
            return 0;
        }
    }
    _window->x11.display = display;

    int screen = DefaultScreen(display);

    int x = _window->wndcnfg.Size.offsetX;
    int y = _window->wndcnfg.Size.offsetY;
    if(_window->wndcnfg.Size.flags & WC_SIZE_XCENTER)
    {
        x = XWidthOfScreen(&screen) / 2 - _window->wndcnfg.Size.width / 2;
    }
    if (_window->wndcnfg.Size.flags & WC_SIZE_YCENTER)
    {
        y = XHeightOfScreen(&screen) / 2 - _window->wndcnfg.Size.height / 2;
    }

    _window->wndcnfg.Size.offsetX = x;
    _window->wndcnfg.Size.offsetY = y;
    _window->x11.window = XCreateSimpleWindow(display, share ? ((WindowStruct*)share)->x11.window : RootWindow(display, screen), x, y, _window->wndcnfg.Size.width, _window->wndcnfg.Size.height, 0, 0, 0);
    
    XStoreName(display, _window->x11.window, _window->wndcnfg.title);

    if (_window->wndcnfg.flags & WC_FLAG_FULLSCREEN)
    {
        Atom wmState = XInternAtom(display, "_NET_WM_STATE", False);
        Atom fullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

        XChangeProperty(display, _window->x11.window, wmState, XA_ATOM, 32, PropModeReplace,
            (unsigned char*)&fullscreen, 1);
    }

    if (!(_window->wndcnfg.flags & WC_FLAG_CAPTION))
    {
        Atom property = XInternAtom(display, "_MOTIF_WM_HINTS", True);
        if (property)
        {
            MotifHints hints;
            hints.flags = MWM_HINTS_DECORATIONS;
            hints.decorations = 0;

            XChangeProperty(display, _window->x11.window, property, property, 32, PropModeReplace, (unsigned char*)&hints, 5);
        }
    }

    if (!(_window->wndcnfg.flags & WC_FLAG_RESIZABLE))
    {
        XSizeHints* size_hints = XAllocSizeHints();
        if (size_hints)
        {
            size_hints->flags = PMinSize | PMaxSize;
            size_hints->min_width = _window->wndcnfg.Size.width;
            size_hints->min_height = _window->wndcnfg.Size.height;
            size_hints->max_width = _window->wndcnfg.Size.width;
            size_hints->max_height = _window->wndcnfg.Size.height;
            XSetWMNormalHints(display, _window->x11.window, size_hints);
            XFree(size_hints);
        }
    }

	_window->x11.atomDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(display, _window->x11.window, &_window->x11.atomDeleteWindow, 1);

    if (_window->wndcnfg.flags & WC_FLAG_ACCEPTDRAGFILES)
    {
        _window->x11.xdnd.XdndTypeList = XInternAtom(display, "XdndTypeList", False);
        _window->x11.xdnd.XdndSelection = XInternAtom(display, "XdndSelection", False);
        _window->x11.xdnd.XdndEnter = XInternAtom(display, "XdndEnter", False);
        _window->x11.xdnd.XdndPosition = XInternAtom(display, "XdndPosition", False);
        _window->x11.xdnd.XdndStatus = XInternAtom(display, "XdndStatus", False);
        _window->x11.xdnd.XdndLeave = XInternAtom(display, "XdndLeave", False);
        _window->x11.xdnd.XdndDrop = XInternAtom(display, "XdndDrop", False);
        _window->x11.xdnd.XdndFinished = XInternAtom(display, "XdndFinished", False);
        _window->x11.xdnd.XdndActionCopy = XInternAtom(display, "XdndActionCopy", False);
        _window->x11.xdnd.XdndActionMove = XInternAtom(display, "XdndActionMove", False);
        _window->x11.xdnd.XdndActionLink = XInternAtom(display, "XdndActionLink", False);
        _window->x11.xdnd.XdndActionAsk = XInternAtom(display, "XdndActionAsk", False);
        _window->x11.xdnd.XdndActionPrivate = XInternAtom(display, "XdndActionPrivate", False);
        _window->x11.xdnd.XtextUriList = XInternAtom((Display*)display, "text/uri-list", False);
        _window->x11.xdnd.XtextPlain = XInternAtom((Display*)display, "text/plain", False);
        _window->x11.xdnd.XdndAware = XInternAtom(display, "XdndAware", False);
        const char myVersion = 5;
        XChangeProperty(display, _window->x11.window, _window->x11.xdnd.XdndAware, 4, 32, PropModeReplace, &myVersion, 1);
    }

    XSelectInput(display, _window->x11.window, ExposureMask | KeyPressMask | StructureNotifyMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask);

    XMapWindow(display, _window->x11.window);
    
	CFWindow window = (CFWindow)_window;
	XSaveContext(display, _window->x11.window, windowContext, (XPointer)window);

    windowCount++;

    event.handler = &_window->wndEvent;
    event.window = window;
    event.type = ONUSERINITIALIZE;
    event.event.ONUSERINITIALIZE.window = window;

    if (((WindowStruct*)window)->wndEvent.OnUserInitialize) ((WindowStruct*)window)->wndEvent.OnUserInitialize(window);

    return window;
}

void getWindowDimensions(CFWindow window, uint32_t* width, uint32_t* height)
{
    assert("null window" && window);
    *width = ((WindowStruct*)window)->wndcnfg.Size.width;
    *height = ((WindowStruct*)window)->wndcnfg.Size.height;
}
void* getWindowUserDataPointer(CFWindow window)
{
    assert("null window" && window);
    return ((WindowStruct*)window)->user_data;
}
void setWindowUserDataPointer(CFWindow window, void* data)
{
    assert("null window" && window);
    ((WindowStruct*)window)->user_data = data;
}
void setWindowTitle(CFWindow window, const char* const title)
{
    assert("null window" && window);
    if (!title) return;
    if (title != ((WindowStruct*)window)->wndcnfg.title)
    {
        ((WindowStruct*)window)->wndcnfg.title = title;
    }
    XStoreName(((WindowStruct*)window)->x11.display, ((WindowStruct*)window)->x11.window, title);
}
void setUserEventCallback(WindowEventHandle* event, CFWindow window)
{
    assert("null window" && window);
    if (!event) return;
    ((WindowStruct*)window)->wndEvent = *event;
}
WindowEventHandle* getWindowEventHandle(CFWindow window)
{
    assert("null window" && window);
    return &((WindowStruct*)window)->wndEvent;
}
unsigned long getX11Window(CFWindow window)
{
    assert("null window" && window);
    return ((WindowStruct*)window)->x11.window;
}
void* getX11Display(CFWindow window)
{
    assert("null window" && window);
    return ((WindowStruct*)window)->x11.display;
}
void setWindowSize(CFWindow window, int width, int height)
{
    assert("null window" && window);
    ((WindowStruct*)window)->wndcnfg.Size.width = width;
    ((WindowStruct*)window)->wndcnfg.Size.height = height;
    XResizeWindow(display, ((WindowStruct*)window)->x11.window, width, height);
}
bool pollEvent()
{
    XNextEvent(display, &xevent);
    CFWindow window = 0;
    if (XFindContext(display, xevent.xany.window, windowContext, (XPointer*)&window)) return false;

    event.window = window;
    event.handler = &((WindowStruct*)window)->wndEvent;

    // Handle events
    if (xevent.type == Expose) {
        // Redraw the window
        event.type = ONUSERRENDER;
        event.event.ONUSERRENDER.window = window;
        if (((WindowStruct*)window)->wndEvent.OnUserRender) ((WindowStruct*)window)->wndEvent.OnUserRender(window);
    }
    else if (xevent.type == KeyPress) {
        // Get the key pressed
        char buffer[32];
        KeySym keysym;
        int len = XLookupString(&xevent.xkey, buffer, sizeof(buffer), &keysym, NULL);

        event.type = ONUSERKEYDOWN;

        if (len > 0) {
            buffer[len] = '\0'; // Null-terminate the string
            
            event.event.ONUSERKEYDOWN.key = buffer;
            if (((WindowStruct*)window)->wndEvent.OnUserKeyDown) ((WindowStruct*)window)->wndEvent.OnUserKeyDown(window, buffer);
        }
        else 
        {
            event.event.ONUSERKEYDOWN.key = keysym;
            if (((WindowStruct*)window)->wndEvent.OnUserKeyDown) ((WindowStruct*)window)->wndEvent.OnUserKeyDown(window, keysym);
        }
    }
    else if (xevent.type == KeyRelease)
    {
        // Get the key released
        char buffer[32];
        KeySym keysym;
        int len = XLookupString(&xevent.xkey, buffer, sizeof(buffer), &keysym, NULL);

        event.type = ONUSERKEYUP;

        if (len > 0) {
            buffer[len] = '\0'; // Null-terminate the string
            event.event.ONUSERKEYUP.key = buffer;
            if (((WindowStruct*)window)->wndEvent.OnUserKeyUp) ((WindowStruct*)window)->wndEvent.OnUserKeyUp(window, buffer);
        }
        else 
        {
            event.event.ONUSERKEYUP.key = keysym;
            if (((WindowStruct*)window)->wndEvent.OnUserKeyUp) ((WindowStruct*)window)->wndEvent.OnUserKeyUp(window, keysym);
		}
    }
    else if (xevent.type == MotionNotify)
    {
        int x = xevent.xmotion.x;
        int y = xevent.xmotion.y;
		int key = xevent.xmotion.state;

        event.type = ONUSERMOUSEMOVE;
        event.event.ONUSERMOUSEMOVE.x = x;
        event.event.ONUSERMOUSEMOVE.y = y;
        event.event.ONUSERMOUSEMOVE.key = key;

        if (((WindowStruct*)window)->wndEvent.OnUserMouseMove) ((WindowStruct*)window)->wndEvent.OnUserMouseMove(window, x, y, key);
    }
    else if (xevent.type == ButtonPress)
    {
        Time now = xevent.xbutton.time;
        unsigned int button = xevent.xbutton.button;
        int x = xevent.xbutton.x;
        int y = xevent.xbutton.y;
        //if (!info.caption && button == 1)
        //{
        //    startX = event.xbutton.x_root; // Mouse X position (relative to screen)
        //    startY = event.xbutton.y_root; // Mouse Y position (relative to screen)
        //    winX = event.xbutton.x_root - event.xbutton.x;
        //    winY = event.xbutton.y_root - event.xbutton.y;
        //}

        

        if (button == last_mouse_click.button &&
            now - last_mouse_click.time <= MAX_DOUBLECLICK_THERSHOLD && 
            now - last_mouse_click.time >= MIN_DOUBLECLICK_THERSHOLD && 
            last_mouse_click.x == x && last_mouse_click.y == y && !last_mouse_click.double_click && button < 4)
        {
            event.type = ONUSERMOUSEDOUBLECLK;
            event.event.ONUSERMOUSEDOUBLECLK.x = x;
            event.event.ONUSERMOUSEDOUBLECLK.y = y;
            event.event.ONUSERMOUSEDOUBLECLK.keyIndex = (int)button;
            last_mouse_click.double_click = true;
            if (((WindowStruct*)window)->wndEvent.OnUserMouseDoubleClick) ((WindowStruct*)window)->wndEvent.OnUserMouseDoubleClick(window, x, y, (int)button);
        }
        else if(button < 4)
        {
            event.type = ONUSERMOUSEDOWN;
            event.event.ONUSERMOUSEDOWN.x = x;
            event.event.ONUSERMOUSEDOWN.y = y;
            event.event.ONUSERMOUSEDOWN.keyIndex = (int)button;
            last_mouse_click.double_click = false;
            if (((WindowStruct*)window)->wndEvent.OnUserMouseDown) ((WindowStruct*)window)->wndEvent.OnUserMouseDown(window, x, y, (int)button);
        }
        else if(button >= 4)
        {
            int dir = 0;
            switch (button)
            {
            case 4:
            {
                dir = 1;
                break;
            }
            case 5:
            {
                dir = -1;
                break;
            }
            case 6:
            {
                // right wheel
                // not supported
                break;
            }
            case 7:
            {
                // left wheel
                // not supported
                break;
            }
            }

            event.type = ONUSERMOUSEWHEEL;
            event.event.ONUSERMOUSEWHEEL.x = x;
            event.event.ONUSERMOUSEWHEEL.y = y;
            event.event.ONUSERMOUSEWHEEL.dir = dir;
            if (((WindowStruct*)window)->wndEvent.OnUserMouseWheel) ((WindowStruct*)window)->wndEvent.OnUserMouseWheel(window, x, y, dir);
        }

        
        last_mouse_click.time = now;
        last_mouse_click.button = button;
        last_mouse_click.x = x;
        last_mouse_click.y = y;
        
    }
    else if (xevent.type == ButtonRelease)
    {
        unsigned int button = xevent.xbutton.button;
        int x = xevent.xbutton.x;
        int y = xevent.xbutton.y;
        /*if (!info.caption && button == 1)
            drag = 0;*/

        event.type = ONUSERMOUSEUP;
        event.event.ONUSERMOUSEUP.x = x;
        event.event.ONUSERMOUSEUP.y = y;
        event.event.ONUSERMOUSEUP.keyIndex = (int)button;

        if (((WindowStruct*)window)->wndEvent.OnUserMouseUp) ((WindowStruct*)window)->wndEvent.OnUserMouseUp(window, x, y, (int)button);
    }
    else if (xevent.type == ConfigureNotify)
    {
        if(xevent.xconfigure.width != ((WindowStruct*)window)->wndcnfg.Size.width || xevent.xconfigure.height != ((WindowStruct*)window)->wndcnfg.Size.height)
        {
            ((WindowStruct*)window)->wndcnfg.Size.width = xevent.xconfigure.width;
            ((WindowStruct*)window)->wndcnfg.Size.height = xevent.xconfigure.height;

            event.type = ONUSERRESIZE;
            event.event.ONUSERRESIZE.width = xevent.xconfigure.width;
            event.event.ONUSERRESIZE.height = xevent.xconfigure.height;

            if (((WindowStruct*)window)->wndEvent.OnUserResize) ((WindowStruct*)window)->wndEvent.OnUserResize(window, xevent.xconfigure.width, xevent.xconfigure.height);
        }
    
        if (xevent.xconfigure.x != ((WindowStruct*)window)->wndcnfg.Size.offsetX || xevent.xconfigure.y != ((WindowStruct*)window)->wndcnfg.Size.offsetY)
        {
            ((WindowStruct*)window)->wndcnfg.Size.offsetX = xevent.xconfigure.x;
            ((WindowStruct*)window)->wndcnfg.Size.offsetY = xevent.xconfigure.y;

            event.type = ONUSERMOVE;
            event.event.ONUSERMOVE.x = xevent.xconfigure.x;
            event.event.ONUSERMOVE.y = xevent.xconfigure.y;

            if (((WindowStruct*)window)->wndEvent.OnUserMove) ((WindowStruct*)window)->wndEvent.OnUserMove(window, xevent.xconfigure.x, xevent.xconfigure.y);
        }
    }
    else if (xevent.type == DestroyNotify) 
    {
        XDestroyWindow(display, xevent.xdestroywindow.window);
    }
    else if(xevent.type == ClientMessage)
    {
        if ((Atom)xevent.xclient.data.l[0] == ((WindowStruct*)window)->x11.atomDeleteWindow)
        {
            windowCount--;
            event.type = ONUSERQUIT;
            event.event.ONUSERQUIT.quitCode = 0;
            if (((WindowStruct*)window)->wndEvent.OnUserQuit) ((WindowStruct*)window)->wndEvent.OnUserQuit(window, event.event.ONUSERQUIT.quitCode);
            if (!windowCount)
            {
                XCloseDisplay(display);
            }
            return false;
        }
        else
            if (xevent.xclient.message_type == ((WindowStruct*)window)->x11.xdnd.XdndDrop)
            {
                XConvertSelection(display, ((WindowStruct*)window)->x11.xdnd.XdndSelection, ((WindowStruct*)window)->x11.xdnd.XtextUriList, ((WindowStruct*)window)->x11.xdnd.XtextUriList, xevent.xclient.window, CurrentTime);
        }
    }
    else if (xevent.type == SelectionNotify) 
    {
        if (xevent.xselection.property != 0L) 
        {
            Atom actual_type;
            int actual_format;
            unsigned long nitems, bytes_after;
            unsigned char* data = NULL;

            XGetWindowProperty(display, xevent.xselection.requestor, ((WindowStruct*)window)->x11.xdnd.XtextUriList, 0, LONG_MAX / 4, True,
                AnyPropertyType, &actual_type, &actual_format,
                &nitems, &bytes_after, &data);
            if (data)
            {
                printf("Dropped data: %s\n", data); // usually file:// URIs
                //if(((WindowStruct*)window)->wndEvent.OnUserDragFile) ((WindowStruct*)window)->wndEvent.OnUserDragFile(window, data, nitems);
                XFree(data);
            }
        }
    }
    return true;
}
Event* CFGetWindowEvent()
{
    return &event;
}

#endif