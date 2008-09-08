//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	gl_view.h
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef GL_VIEW_H
#define GL_VIEW_H
//--------------------------------------------------------------
#include <GL/glx.h>
//--------------------------------------------------------------
class GLView
{
public:
	GLView(class AppWindow* win);
	~GLView();

	virtual unsigned int draw();
	virtual unsigned int tick(unsigned long usec);
	virtual void reshape(unsigned int width, unsigned int height);
	void swap_buffers();
	virtual bool event_handler(Display* dpy, Window window, XEvent& event);

	static bool check_support(Screen *screen, const char *window_desc, Visual *visual);
	static Visual* choose_visual(Display *dpy, int screen_num);

private:
	GLXContext glx_context;
	Display *dpy;
	Window window;
};
//--------------------------------------------------------------
#endif//GL_VIEW_H
//--------------------------------------------------------------

