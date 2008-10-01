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

class Matrix	// Simply Matrix effect
{
protected:
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

	class Strip
	{
	public:
		Strip(unsigned int n, T2F_V3F_C4F* interleaved_arrays, GLfloat ax, GLfloat ay, GLfloat az, GLfloat spinner, GLfloat wave);
		~Strip();

		void draw(GLint* first, GLsizei* count);
		void tick(unsigned long usec, bool update);

	private:
	
		T2F_V3F_C4F*	array;

		GLfloat x, y, z;
		GLfloat size;

		float ph;
		float r;

		float angle;
	

		GLfloat spinner_begin;
		GLfloat spinner_end;
		GLfloat spinner_speed;

		int end_glyphs;
		unsigned int n_glyphs;

		GLfloat wave;
		GLfloat wave_speed;
	};
public:
	Matrix(unsigned int ns, unsigned int ng, TextureAtlas::Texture* texture);
	~Matrix();
	
	void draw();
	void tick(unsigned long usec);
		
	virtual void set_video(const VideoBuffer* buffer, int widht, int height);	
	
	virtual void pre_draw();
	virtual void post_draw();
	
	TextureAtlas::Texture* letter;

protected:

	T2F_V3F_C4F* 	interleaved_arrays;
	GLint*		firsts;
	GLsizei*	counts;


	unsigned int nstrips;	// Number of strips
	unsigned int nglyphs;	// Number of glyphs
	Strip** strips;
	Random grid_random;
	Timer glyph_update;
};


class MatrixVideo:public Matrix	// Matrix and Video effect
{
public:
	MatrixVideo(unsigned int ns, unsigned int ng, TextureAtlas::Texture* texture);
	~MatrixVideo();

	
	virtual void set_video(const VideoBuffer* buffer, int widht, int height);	
	
	virtual void pre_draw();
	virtual void post_draw();
	
protected:
	T2F* video_st;			// Texture coords
	const VideoBuffer* video;	// Texture instance
};

class MatrixVideoFX:public MatrixVideo	// Matrix Video with Vertex Shader FX
{
public:
	MatrixVideoFX(unsigned int ns, unsigned int ng, TextureAtlas::Texture* texture);
	~MatrixVideoFX();

	virtual void pre_draw();
	virtual void post_draw();
	
protected:
	GPU_Program* program;
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

