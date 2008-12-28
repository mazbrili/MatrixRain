//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	scene.cpp
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#include "matrix.h"
#include "options.h"
#include "scene.h"
#include "stuff.h"
//--------------------------------------------------------------
VideoScreen::VideoScreen(float w, float h):video(NULL),width(w), height(h)
{
}

VideoScreen::~VideoScreen()
{
}

void VideoScreen::draw()
{	
	glTranslatef(-32.0f, -24.0f, -30.0f);
	
	if(video)
	{
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		video->bind(GL_MODULATE);
	}
	glBegin(GL_QUADS);
		glTexCoord2f(video->s, video->t); glVertex3f(0.0f, 0.0f, 0.0f);
		glTexCoord2f(video->s, 0.0f);	glVertex3f(0.0f, height, 0.0f);
		glTexCoord2f(0.0f, 0.0f);	glVertex3f(width, height, 0.0f);
		glTexCoord2f(0.0f, video->t);	glVertex3f(width, 0.0f, 0.0f);
	glEnd();
}

void VideoScreen::tick(unsigned long usec)
{
}

float Scene::user_a = 0.0f;
float Scene::user_b = 0.0f;

Scene::Scene(class AppWindow* win, Capture* capture):GLView(win), atlas(1), frames_stack(NULL), matrix(NULL)
{
	if(capture != NULL/*capturing_enable*/)
	{
		target << *capture;
		frames_stack = new VideoBuffer(target, 0, 10000);

		if( Options::get("--no-shaders") || version() <= "2.0")
		{
			matrix = new MatrixVideo(128, 112, atlas[0], frames_stack, target.width(), target.height());
		}
		else
		{
			matrix = new MatrixVideoFX(128, 112, atlas[0], frames_stack, target.width(), target.height());
		}
	}
	else matrix = new Matrix(128, 112, atlas[0]);
}

Scene::~Scene()
{
	delete frames_stack;
	delete matrix;
}

unsigned int Scene::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(-32.0,24.0,-25.0f);

	matrix->draw();
	return 0;
}

unsigned int Scene::tick(unsigned long usec)
{	
	Waiter::tick();

	if(frames_stack)
	{
		frames_stack->update(target, usec);
	}

	matrix->tick(usec);
	return 0;
}

bool Scene::event_handler(Display* dpy, Window window, XEvent& event)
{
	if (event.xany.type == KeyPress)
	{
		KeySym keysym;
		char c = 0;
		XLookupString (&event.xkey, &c, 1, &keysym, 0);
		
		switch(event.xkey.keycode)
		{
			case 98:	// up
				user_a += 0.5f;
			break;

			case 104:	// down
				user_a -= 0.5f;
			break;

			case 100:	// left
				user_b -= 10.0f;
			break;

			case 102:	// right
				user_b += 10.0f;
			break;
		}
	}

	GLView::event_handler(dpy, window, event);

	return false;
}
//--------------------------------------------------------------

