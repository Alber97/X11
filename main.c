#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <bits/types/struct_itimerspec.h>

#define COLLISION 1
#define NO_COLLISION 0

typedef struct
{
    int radius;
    int x;
    int y;
    int mass;
    int stiffness;
} circleType;

typedef struct
{
    int X;
    int Y;
    int Width;
    int Height;
} entity;

int CheckCollision(int cRad, int cX, int cY, int bX, int bY, int bW, int bH)
{
    if ((abs(cX - bX) < 50) && (abs(cY - bY) < 50))
        return COLLISION;

    return NO_COLLISION;
}

int main()
{
    long long iteration = 0;
    short speedFactor = 1;

    Display* MainDisplay = XOpenDisplay(0);
    Window RootWindow = XDefaultRootWindow(MainDisplay);

    int DefaultScreen = DefaultScreen(MainDisplay);
    GC Context = XDefaultGC(MainDisplay, DefaultScreen);

    int WindowX = 0;
    int WindowY = 0;
    int WindowWidth = 800;
    int WindowHeight = 600;
    int BorderWidth = 0;
    int WindowDepth = CopyFromParent;
    int WindowClass = CopyFromParent;
    Visual* WindowVisual = CopyFromParent;

    int AttributeValueMask = CWBackPixel | CWEventMask;
    XSetWindowAttributes WindowAttributes = {};
    WindowAttributes.background_pixel = 0xffffccaa;
    WindowAttributes.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask | ExposureMask;

    Window MainWindow = XCreateWindow(MainDisplay, RootWindow,
            WindowX, WindowY, WindowWidth, WindowHeight,
            BorderWidth, WindowDepth, WindowClass, WindowVisual,
            AttributeValueMask, &WindowAttributes);

    XMapWindow(MainDisplay, MainWindow);

    XStoreName(MainDisplay, MainWindow, "Moving rectangle. Use arrow keys to move.");

    Atom WM_DELETE_WINDOW = XInternAtom(MainDisplay, "WM_DELETE_WINDOW", False);
    if(!XSetWMProtocols(MainDisplay, MainWindow, &WM_DELETE_WINDOW, 1)) {
        printf("Couldn't register WM_DELETE_WINDOW property \n");
    }


    entity Box = {};
    Box.Width = 50;
    Box.Height = 50;
    Box.X = WindowWidth/2 - Box.Width/2;
    Box.Y = WindowHeight/2 - Box.Height/2;

    circleType Circle = {};
    Circle.radius = 60;
    Circle.x = WindowWidth/4 - Circle.radius / 2;
    Circle.y = WindowHeight/2 - Circle.radius / 2;

    int StepSize = 5;

    int IsWindowOpen = 1;
    while(IsWindowOpen) {
        XEvent GeneralEvent = {};

        iteration++;
        speedFactor = (short)(iteration * 0.01);

        if (speedFactor == 0)
            speedFactor = 1;

        if (Circle.x > Box.X)
            Circle.x -= speedFactor;
        else if (Circle.x < Box.X)
            Circle.x += speedFactor;

        if (Circle.y > Box.Y)
            Circle.y -= speedFactor;
        else if (Circle.y < Box.Y)
            Circle.y += speedFactor;

        if (CheckCollision(Circle.radius, Circle.x, Circle.y, Box.X, Box.Y, Box.Width, Box.Height) == COLLISION)
            return 0;

        XNextEvent(MainDisplay, &GeneralEvent);

        switch(GeneralEvent.type) {
            case KeyPress:
            case KeyRelease:
            {
                XKeyPressedEvent *Event = (XKeyPressedEvent *)&GeneralEvent;
                if(Event->keycode == XKeysymToKeycode(MainDisplay, XK_Escape))
                {
                    IsWindowOpen = 0;
                }

                if(Event->keycode == XKeysymToKeycode(MainDisplay, XK_Down))
                {
                    Box.Y += StepSize;
                }
                else if(Event->keycode == XKeysymToKeycode(MainDisplay, XK_Up))
                {
                    Box.Y -= StepSize;
                }
                else if(Event->keycode == XKeysymToKeycode(MainDisplay, XK_Right))
                {
                    Box.X += StepSize;
                }
                else if(Event->keycode == XKeysymToKeycode(MainDisplay, XK_Left))
                {
                    Box.X -= StepSize;
                }
            } break;

            case ClientMessage:
            {
                XClientMessageEvent *Event = (XClientMessageEvent *) &GeneralEvent;
                if((Atom)Event->data.l[0] == WM_DELETE_WINDOW) {
                    XDestroyWindow(MainDisplay, MainWindow);
                    IsWindowOpen = 0;
                }
            } break;

        }

        XClearWindow(MainDisplay, MainWindow);
        XFillRectangle(MainDisplay, MainWindow, Context, Box.X, Box.Y, Box.Width, Box.Height);
        XFillArc(MainDisplay, MainWindow, Context, Circle.x, Circle.y, Circle.radius, Circle.radius, 0, 360*64);
    }
}