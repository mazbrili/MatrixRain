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

#include "stuff.h"
//--------------------------------------------------------------
class GLView
{
public:

	struct T2F_V3F_C4F
	{
		union	// T2F
		{
			struct{ GLfloat st[2]; };
			struct{ GLfloat s,t;   };
		};

		union	// V3F
		{
			struct{ GLfloat xyz[3]; };
			struct{ GLfloat x,y,z;  };
		};				

		union	// C4F
		{
			struct{ GLfloat rgba[4]; };
			struct{ GLfloat r,g,b,a; };
		};
	};

	struct T2F
	{
		union	// T2F
		{
			struct{ GLfloat st[2]; };
			struct{ GLfloat s,t;  };
		};
	};

	struct V3F
	{
		union	// V3F
		{
			struct{ GLfloat xyz[3]; };
			struct{ GLfloat x,y,z;  };
		};
	};

	struct C4F
	{
		union	// C4F
		{
			struct{ GLfloat rgba[4]; };
			struct{ GLfloat r,g,b,a; };
		};
	};


	GLView(class AppWindow* win);
	virtual ~GLView();

	virtual unsigned int draw();
	virtual unsigned int tick(unsigned long usec);
	virtual void reshape(unsigned int width, unsigned int height);
	void swap_buffers();
	virtual bool event_handler(Display* dpy, Window window, XEvent& event);

	static bool check_support(Screen *screen, const char *window_desc, Visual *visual);
	static Visual* choose_visual(Display *dpy, int screen_num);

	const Version& version()const	{ return gl_version;  }
	const GLubyte* renderer()const	{ return glGetString(GL_RENDERER); }
	const GLubyte* vendor()const	{ return glGetString(GL_VENDOR);   }

private:
	GLXContext glx_context;
	Display *dpy;
	Window window;
	Version gl_version;
};
//--------------------------------------------------------------
#endif//GL_VIEW_H
//--------------------------------------------------------------

