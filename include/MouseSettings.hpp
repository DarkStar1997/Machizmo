#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <unistd.h>

Display* dpy=XOpenDisplay(0);
int scr=XDefaultScreen(dpy);
Window root_window=XRootWindow(dpy, scr);

struct Mouse
{
    int max_x, max_y, delay;
    void (*ScalingFunction)(double&, double&);
    Mouse()
    {
        max_y = DisplayHeight(dpy, scr);
        max_x = DisplayWidth(dpy, scr);
        delay = 50;
    }
    void moveTo(double x, double y, bool ctrl = false)
    {
        if(ctrl)
            ScalingFunction(x, y);
        XWarpPointer(dpy, 0L, root_window, 0, 0, 0, 0, x, y);
        XFlush(dpy);
        usleep(delay);
    }
    void rightClick()
    {
        XTestFakeButtonEvent(dpy, 3, True, 0);
        XFlush(dpy);
        XTestFakeButtonEvent(dpy, 3, False, 0);
        XFlush(dpy);
        usleep(delay);
    }
    void leftClick()
    {
        XTestFakeButtonEvent(dpy, 2, True, 0);
        XFlush(dpy);
        XTestFakeButtonEvent(dpy, 2, False, 0);
        XFlush(dpy);
        usleep(delay);
    }
    void setScalingFunction(void (*ptr)(double&, double&))
    {
        ScalingFunction = ptr;
    }
};
