//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	scene.h
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef SCENE_H
#define SCENE_H
//--------------------------------------------------------------
#include <cmath>

#include "stuff.h"
#include "gl_view.h"
#include "matrix.h"
#include "texture_atlas.h"
#include "video_buffer.h"
//--------------------------------------------------------------
class VideoScreen
{
public:
	VideoScreen(float w, float h);
	~VideoScreen();

	void set_video(const VideoBuffer* buffer){ video = buffer; }
	
	void draw();
	void tick(unsigned long usec);
private:

	const VideoBuffer* video;
	float width;
	float height;
};

class Scene:public GLView
{
public:
	Scene(class AppWindow* win, bool capturing_enable);
	~Scene();
	
	unsigned int draw();
	unsigned int tick(unsigned long usec);
	bool event_handler(Display* dpy, Window window, XEvent& event);

	TextureAtlas atlas;
	VideoScreen* screen;	// Real Time Video
	Matrix* matrix;

	static float user_a;
	static float user_b;
};
//--------------------------------------------------------------
#endif//SCENE_H
//--------------------------------------------------------------

