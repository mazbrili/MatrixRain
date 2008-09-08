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
#include "stuff.h"
#include "gl_view.h"
#include "gpu_program.h"
#include "matrix_img.h"
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

class Matrix
{	
	class Strip
	{
	public:
		Strip(unsigned int n, GLfloat ax, GLfloat ay, GLfloat az, GLfloat spinner, GLfloat wave);
		~Strip();

		void draw();
		void tick(unsigned long usec, bool update);

	private:
		
		GLfloat x, y, z;
		GLfloat size;
		GLfloat dx, dy, dz;

		GLfloat spinner_begin;
		GLfloat spinner_end;
		GLfloat spinner_speed;

		int* glyphs;
		int end_glyphs;
		unsigned int n_glyphs;

		GLfloat wave_y;
		GLfloat wave_speed;
	};
public:
	Matrix(unsigned int n, int widht, int height);
	~Matrix();
	void set_video(const VideoBuffer* buffer);
	
	void draw();
	void tick(unsigned long usec);
	
	const VideoBuffer* video;
	TextureAtlas::Texture* letter;
	GPU_Program* program;

private:

	unsigned int num;
	Strip** strips;
	Random grid_random;
	Timer glyph_update;
	float video_res[4];
};


class Scene:public GLView
{
public:
	Scene(class AppWindow* win);
	~Scene();
	
	unsigned int draw();
	unsigned int tick(unsigned long usec);
	bool event_handler(Display* dpy, Window window, XEvent& event);

	TextureAtlas atlas;
	VideoScreen screen;	// Real Time Video
	Matrix	matrix;

	static float user_a;
	static float user_b;
};
//--------------------------------------------------------------
#endif//SCENE_H
//--------------------------------------------------------------

