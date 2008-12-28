//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	app_window.h
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef APP_WINDOW
#define APP_WINDOW
//--------------------------------------------------------------
#include <X11/Xlib.h>
//--------------------------------------------------------------
class AppWindow
{
public:
	AppWindow();
	~AppWindow();

	bool process_events(class GLView* view);
	void clear_background(unsigned int long color);

	Display* display;
	Window window;
private:
	static int x_error_handler(Display *dpy, XErrorEvent *error);
	static Bool map_notify_event (Display *dpy, XEvent *event, XPointer window);
	static Window root_window(Screen *screen);

	Atom XA_WM_PROTOCOLS;
	Atom XA_WM_DELETE_WINDOW;
	bool own_window;
};
//--------------------------------------------------------------
#endif//APP_WINDOW
//--------------------------------------------------------------

