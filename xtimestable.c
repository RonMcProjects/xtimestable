/* 
 * gcc -Wall xtimestable.c -lm -lX11 -o xtimestable
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#define SCALE_FACTOR 340
#define PI 3.1415926536

/*
 * Constants
 */
char WINDOW_NAME[] = __FILE__;
char ICON_NAME[] = __FILE__;

/*
 * Globals
 */
Display *display;
int screen;
Window main_window;
GC gc;
unsigned long foreground, background;
XColor RGB_color, hardware_color;    /* added for color. */
Colormap color_map;      /* added for color. */

/*
 * Connect to the server and get the display device
 * and the screen number.
 */
void initX()
{
    int depth;                  /* depth of the color map. */

    /*
     * Set the display name from the environment variable DISPLAY.
     */
    display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        fprintf(stderr, "Unable to open display %s\n", XDisplayName(NULL));
        exit(1);
    }
    screen = DefaultScreen(display);

    /* Find the depth of the colour map. */
    depth = DefaultDepth(display, screen);

    /* Set the default foreground and background, in case we cannot use colour. */
    foreground = BlackPixel(display, screen);
    background = WhitePixel(display, screen);

    if (depth > 1)              /* not monochrome */
    {
        color_map = DefaultColormap(display, screen);
        if (XLookupColor(display, color_map, "black", &RGB_color, &hardware_color) != 0
            && XAllocColor(display, color_map, &hardware_color) != 0)
            background = hardware_color.pixel;

        if (XLookupColor(display, color_map, "LightSlateBlue", &RGB_color, &hardware_color) != 0
            && XAllocColor(display, color_map, &hardware_color) != 0)
            foreground = hardware_color.pixel;

    }
}

/*
 * Opens a window on the display device, and returns
 * the windows ID.
 */
Window openWindow(x, y, width, height, border_width, argc, argv)
int x, y,                       /* co-ords of the upper left corner in pixels. */
width, height,                  /* size of the windows in pixels. */
    border_width;               /* the border width is not included in the other dimensions. */
int argc;
char **argv;
{
    Window new_window;
    XSizeHints size_hints;

    /* now create the window. */
    new_window = XCreateSimpleWindow(display,
                                     DefaultRootWindow(display),
                                     x, y, width, height, border_width, foreground, background);

    /* set up the size hints for the window manager. */
    size_hints.x = x;
    size_hints.y = y;
    size_hints.width = width;
    size_hints.height = height;

    size_hints.flags = PPosition | PSize;

    /* and state what hints are included. */
    XSetStandardProperties(display, new_window, WINDOW_NAME, ICON_NAME, None,   /* no icon map */
                           argv, argc, &size_hints);

    /* Return the window ID. */
    return (new_window);
}

/*
 * Create a graphics context using default values and,
 * return it in the pointer gc.
 */
GC getGC()
{
    GC gc;
    XGCValues gcValues;

    gc = XCreateGC(display, main_window, (unsigned long)0, &gcValues);

    XSetBackground(display, gc, background);
    XSetForeground(display, gc, foreground);

    return (gc);
}

/*
 * Terminate the program gracefully.
 */
void quitX()
{
    XCloseDisplay(display);
    exit(0);
}

int display_something(int iter, int factor)
{
    double ang1, ang2;
    double point;
    double nextpoint;
    double x1, y1;
    double x2, y2;
    double x1_scaled, y1_scaled;
    double x2_scaled, y2_scaled;
    double numpoints = (double)iter;
    double multiplier = (double)factor;
    char factor_str[10];
    int length_str;

    length_str = sprintf(factor_str, "%d", factor);
    XDrawArc(display, main_window, gc, 0, 0, SCALE_FACTOR*2, SCALE_FACTOR*2, 0, 360*64);
    XDrawString(display, main_window, gc, SCALE_FACTOR*2 - 30, SCALE_FACTOR*2 - 30, factor_str, length_str);
    for (point = 1; point <= numpoints; point++)
    {
        nextpoint = point * multiplier;
        ang1 = 2.0*PI/numpoints * point;
        ang2 = 2.0*PI/numpoints * nextpoint;
        x1 = cos(ang1);
        y1 = sin(ang1);
        x1_scaled = x1 * SCALE_FACTOR + SCALE_FACTOR;
        y1_scaled = -1.0 * y1 * SCALE_FACTOR + SCALE_FACTOR;
        x2 = cos(ang2);
        y2 = sin(ang2);
        x2_scaled = x2 * SCALE_FACTOR + SCALE_FACTOR;
        y2_scaled = -1.0 * y2 * SCALE_FACTOR + SCALE_FACTOR;
        XDrawLine(display, main_window, gc, (int)x1_scaled, (int)y1_scaled, (int)x2_scaled, (int)y2_scaled);
        XFlush(display);
    }

    return (0);
}

int main(argc, argv)
int argc;
char **argv;
{
    int done = 0;
    int iter = 10;
    int factor = 2;
    int animate = -1;
    useconds_t delay = 500000;
    XEvent event;

    initX();
    main_window = openWindow(10, 20, SCALE_FACTOR*2+1, SCALE_FACTOR*2+1, 5, argc, argv);
    gc = getGC();

    /* Display the window on the screen. */
    XMapWindow(display, main_window);

    if (argc >= 2)
    {
        iter = atoi(argv[1]);
    }
    if (argc >= 3)
    {
        factor = atoi(argv[2]);
    }
    if (argc >= 4)
    {
        animate = atoi(argv[3]);
    }
    if (argc == 5)
    {
        delay = atoi(argv[4]);
    }

    /* flush event queue */
    XSelectInput(display, main_window, ExposureMask);
    XNextEvent(display, &event);
    do {
        XClearWindow(display, main_window);
        display_something(iter, factor);
        if (animate == -1)
            display_something(iter, factor);

        XSelectInput(display, main_window, KeyPressMask | ButtonPressMask);

        if (factor == 2) usleep(500000);
        if (animate == 0)
        {
            XNextEvent(display, &event);
            if (event.xkey.keycode == 0x18) /* 'q' */
            {
                break;
            }
            else if (event.xkey.keycode == 0x21) /* 'p' */
            {
                factor -= 1;
            }
            else
            {
                factor += 1;
            }
        }
        else
        {
            factor += animate;
            usleep(delay);
        }
    } while ((animate != -1) && (factor <= iter));

    if (event.xkey.keycode != 0x18)
    {
        while (!done)
        {
            XNextEvent(display, &event);
            if ((event.type == KeyPress) || (event.type == ButtonPress))
                done = 1;
        }
    }
    quitX();

    return (0);
}
