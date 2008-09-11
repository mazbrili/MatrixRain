//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	gl_view.cpp
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#include <cstdio>
#include <cmath>
#include <cstdlib>

#include "gl_view.h"
#include "app_window.h"

extern FILE* log22;


//--------------------------------------------------------------
GLView::GLView(AppWindow* win)
{
	dpy = win->display;
	window = win->window;
	XWindowAttributes xgwa;
	XGetWindowAttributes (dpy, window, &xgwa);
	Screen *screen = xgwa.screen;
	Visual *visual = xgwa.visual;
	XVisualInfo vi_in, *vi_out;
	int out_count;

	vi_in.screen = DefaultScreen(dpy);
	vi_in.visualid = XVisualIDFromVisual (visual);
	vi_out = XGetVisualInfo (dpy, VisualScreenMask|VisualIDMask, &vi_in, &out_count);
	if (! vi_out)
	{
//		fprintf(::log22,"%s %d vi_out == false\n", __FILE__, __LINE__);	
//		fflush(log22);
		abort ();
	}

	{
		XSync (dpy, False);
	//	orig_ehandler = XSetErrorHandler (BadValue_ehandler);
		glx_context = glXCreateContext (dpy, vi_out, 0, True);

//		fprintf(::log22,"%s %d GL context %X\n", __FILE__, __LINE__, glx_context);
//		fflush(log22);
		XSync (dpy, False);
	//	XSetErrorHandler (orig_ehandler);
	//	if (got_error)
	//	glx_context = 0;
	}

	XFree((char *) vi_out);

	if (!glx_context)
	{
	fprintf(stderr, "couldn't create GL context for visual 0x%x.\n", (unsigned int) XVisualIDFromVisual (visual));
	exit(1);
	}

	glXMakeCurrent (dpy, window, glx_context);

	{
		GLboolean rgba_mode = 0;
		glGetBooleanv(GL_RGBA_MODE, &rgba_mode);
		if (!rgba_mode)
		{
			glIndexi (WhitePixelOfScreen (screen));
			glClearIndex (BlackPixelOfScreen (screen));
		}
	}

	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);
//	glDisable(GL_CULL_FACE);
	glBlendFunc (GL_ONE_MINUS_SRC_ALPHA,  GL_SRC_ALPHA);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glXSwapBuffers(dpy, window);

	reshape( xgwa.width, xgwa.height );
}

GLView::~GLView()
{
	glXDestroyContext( dpy, glx_context );
}

unsigned int GLView::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();
	glTranslatef(0,0,-20);
	glBegin(GL_TRIANGLES);		
		glColor3f(0.0f, 1.0, 0.0f); glVertex3f(0.0, 10.0, 0.0);
		glColor3f(1.0f, 0.0, 0.0f); glVertex3f(0.0, 0.0, 0.0);
		glColor3f(0.0f, 0.0, 1.0f); glVertex3f(10.0, 0.0, 0.0);
	glEnd();

	glXSwapBuffers(dpy, window);
	return 0;
}

unsigned int GLView::tick(unsigned long usec)
{	
	return 0;
}

void GLView::reshape(unsigned int width, unsigned int height)
{
	glViewport (0, 0, (GLint) width, (GLint) height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLfloat aspect = (GLfloat) width/(GLfloat) height;

	float fov = 80.0f;
	double cotangent = 1.0/tan((fov /2.0) * 3.1415926 / 180.0);
	double near = 1.0;
	double far  = 1000.0;

	double b = near/cotangent; 
	double a = aspect*b;

	glFrustum(-a, a, -b, b, near, far);
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT);
}

void GLView::swap_buffers()
{
	glXSwapBuffers(dpy, window);	
}

bool GLView::event_handler(Display* dpy, Window window, XEvent& event)
{
	if (event.xany.type == ConfigureNotify)
	{
		if (event.xany.window == window)
			reshape(event.xconfigure.width, event.xconfigure.height);
	}

	if (event.xany.type == ButtonPress && event.xbutton.button == Button1)
	{
		//mp->button_down_p = True;
		return true;
	}
	else if (event.xany.type == ButtonRelease && event.xbutton.button == Button1)
	{
		//mp->button_down_p = False;
		return true;
	}

	return false;
}



bool GLView::check_support(Screen *screen, const char *window_desc, Visual *visual)
{
	Display *dpy = DisplayOfScreen (screen);
	XVisualInfo vi_in, *vi_out;
	int out_count;
	unsigned int id;


	for (int i = 0; i < ScreenCount (dpy); i++)
	{
		if (ScreenOfDisplay (dpy, i) == screen)
		{
			vi_in.screen = i;
			break;
		}
	}

	vi_in.visualid = XVisualIDFromVisual (visual);
	vi_out = XGetVisualInfo (dpy, (VisualScreenMask | VisualIDMask),
	&vi_in, &out_count);

	if (! vi_out) abort ();

	int value = False;
	int status = glXGetConfig (dpy, vi_out, GLX_USE_GL, &value);

	id = (unsigned int) vi_out->visualid;
	XFree ((char *) vi_out);

	if (status == GLX_NO_EXTENSION)
	{
		fprintf (stderr, "display \"%s\" does not support the GLX extension.\n", DisplayString (dpy));
		return false;
	}
	else if (status == GLX_BAD_VISUAL || value == False)
	{
		fprintf (stderr, "%s's visual 0x%x does not support the GLX extension.\n", window_desc, id);
		return false;
	}
	
	return true;
}

Visual* GLView::choose_visual(Display *display, int screen_num)
{
	enum {
		R=GLX_RED_SIZE,
		G=GLX_GREEN_SIZE,
		B=GLX_BLUE_SIZE,
		A=GLX_ALPHA_SIZE,
		D=GLX_DEPTH_SIZE,
		I=GLX_BUFFER_SIZE,
		L=GLX_LEVEL,
		DB=GLX_DOUBLEBUFFER,
		ST=GLX_STENCIL_SIZE,
	};

	int attrs[][20] = {
	{ GLX_RGBA, R,8, G,8, B,8, A,8, D,8, DB, ST,1, 0 }, /* rgb double, stencil */
	{ GLX_RGBA, R,8, G,8, B,8,      D,8, DB, ST,1, 0 }, /* rgb double, stencil */
	{ GLX_RGBA, R,4, G,4, B,4,      D,4, DB, ST,1, 0 },
	{ GLX_RGBA, R,2, G,2, B,2,      D,2, DB, ST,1, 0 },
	{ GLX_RGBA, R,8, G,8, B,8,      D,8, DB,       0 }, /* rgb double */
	{ GLX_RGBA, R,4, G,4, B,4,      D,4, DB,       0 },
	{ GLX_RGBA, R,2, G,2, B,2,      D,2, DB,       0 },
	{ GLX_RGBA, R,8, G,8, B,8,      D,8,           0 }, /* rgb single */
	{ GLX_RGBA, R,4, G,4, B,4,      D,4,           0 },
	{ GLX_RGBA, R,2, G,2, B,2,      D,2,           0 },
	{ I, 8,                         D,8, DB,       0 }, /* cmap double */
	{ I, 4,                         D,4, DB,       0 },
	{ I, 8,                         D,8,           0 }, /* cmap single */
	{ I, 4,                         D,4,           0 },
	{ GLX_RGBA, R,1, G,1, B,1,      D,1,           0 }  /* monochrome */
	};

	for (unsigned int i = 0; i < sizeof(attrs)/sizeof(*attrs); i++)
	{
		XVisualInfo* vi = glXChooseVisual (display, screen_num, attrs[i]);
		if (vi)
		{
			Visual* v = vi->visual;
			XFree (vi);
			return v;
		}
	}

	return 0;
}
//--------------------------------------------------------------

