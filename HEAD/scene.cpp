//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	scene.cpp
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#include <cmath>

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

Matrix::Matrix(unsigned int ns, unsigned int ng, TextureAtlas::Texture* texture): letter(texture),
	interleaved_arrays(NULL), firsts(NULL), counts(NULL),
	nstrips(ns), nglyphs(ng),
	strips(NULL), grid_random(324467)
{
	unsigned int strip_pack = nglyphs * 4;
	interleaved_arrays = new T2F_V3F_C4F[nstrips * strip_pack];
	firsts = new GLint[nstrips];
	counts = new GLsizei[nstrips];

	strips = new Strip*[nstrips];

	fprintf(stderr, "num bytes: %i\n", nstrips * strip_pack * sizeof(T2F_V3F_C4F));
	for(unsigned int i=0; i<nstrips; i++)
	{
		strips[i] = new Strip(strip_pack, &interleaved_arrays[i*strip_pack], float(i)/2.0f, -grid_random(3.0f), 0, grid_random(5.0f)+5.0f, grid_random(5.0f)+5.0f);
	}
}

Matrix::~Matrix()
{
	for(unsigned int i=0; i<nstrips; i++)
	{
		delete strips[i];
	}
	delete[] strips;

	delete[] firsts;
	delete[] counts;
	delete[] interleaved_arrays;
}	

void Matrix::draw()
{	
	glLoadIdentity();
	glTranslatef(-32.0,24.0,-25.0f);

//	glTranslatef(0.0, 0.0, -75.0f);

//	glTranslatef(0,-2.5f,-20.0f);
//	glRotatef(15.0f, 1.0f, 0.0f, 0.0f);

	pre_draw();
	
	for(unsigned int i=0; i<nstrips; i++)
	{
		strips[i]->draw(&firsts[i], &counts[i]);
		firsts[i] += (i * nglyphs<<2);
	}

	glMultiDrawArrays(GL_TRIANGLE_STRIP, firsts, counts, nstrips);

	post_draw();
}

void Matrix::tick(unsigned long usec)
{
	bool update = false;
	glyph_update.tick();

	if(80000 < glyph_update.time() )
	{
		update = true;
		glyph_update.reset();	
	}

	for(unsigned int i=0; i<nstrips; i++)
	{
		strips[i]->tick(usec, update);
	}
}

void Matrix::set_video(const VideoBuffer* buffer, int widht, int height)
{
}

void Matrix::pre_draw()
{	
	// No video & fixed pipeline
	glPushAttrib(GL_TEXTURE_BIT|GL_ENABLE_BIT);	// Save server state
	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);	// Save client state	
	
	glEnable(GL_BLEND);

	glActiveTexture(GL_TEXTURE0);	
	glEnable(GL_TEXTURE_2D);
	letter->bind();
	glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_DECAL);

	glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(2, GL_FLOAT, sizeof(T2F_V3F_C4F), &interleaved_arrays[0].st);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glColorPointer	 (4, GL_FLOAT, sizeof(T2F_V3F_C4F), &interleaved_arrays[0].rgba);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer  (3, GL_FLOAT, sizeof(T2F_V3F_C4F), &interleaved_arrays[0].xyz);
	glEnableClientState(GL_VERTEX_ARRAY);
}

void Matrix::post_draw()
{
	glPopClientAttrib();	// Load client state	
	glPopAttrib();		// Load server state
}

Matrix::Strip::Strip(unsigned int n, T2F_V3F_C4F* interleaved_arrays, GLfloat ax, GLfloat ay, GLfloat az, GLfloat spinner, GLfloat wspeed):array(interleaved_arrays), x(ax), y(ay), z(az), n_glyphs(n/4)
{
	size = 0.4f;

	ph = 0.2f;
	r = 0.95f;
	angle = 0.0f;
	

	spinner_speed = spinner;
	spinner_begin = 0.0f;
	spinner_end = 0.0f;

	wave = 0.0f;
	wave_speed = wspeed;

	GLfloat yi = y;
	for(unsigned int i=0; i<n; i+= 4)
	{
		GLfloat a = 1.0 - cosf(float((int(wave - (i/4))%80))*(3.1415926f/180.0f));

		GLfloat s = float(rand()%32)/32.0f;
		
		array[i].s = s;
		array[i].t = 1.0f;
		array[i].x = x;
		array[i].y = yi;
		array[i].z = z;
		array[i].r = 0.0f;
		array[i].g = 1.0f;
		array[i].b = 0.0f;
		array[i].a = a;
		
		array[i+1].s = s+1.0f/32.0f;
		array[i+1].t = 1.0f;
		array[i+1].x = x+size;
		array[i+1].y = yi;
		array[i+1].z = z;
		array[i+1].r = 0.0f;
		array[i+1].g = 1.0f;
		array[i+1].b = 0.0f;
		array[i+1].a = a;

		array[i+2].s = s;
		array[i+2].t = 0.0f;
		array[i+2].x = x;
		array[i+2].y = yi-size;
		array[i+2].z = z;
		array[i+2].r = 0.0f;
		array[i+2].g = 1.0f;
		array[i+2].b = 0.0f;
		array[i+2].a = a;
				
		array[i+3].s = s+1.0f/32.0f;
		array[i+3].t = 0.0f;
		array[i+3].x = x+size;
		array[i+3].y = yi-size;
		array[i+3].z = z;
		array[i+3].r = 0.0f;
		array[i+3].g = 1.0f;
		array[i+3].b = 0.0f;
		array[i+3].a = a;

		yi -= size;
	}
}

float f(float x);
float phi(float x, float n);
float sigm(float x, float a);

Matrix::Strip::~Strip()
{
}

void Matrix::Strip::draw(GLint* first, GLsizei* count)
{
	*first = 0;
	*count = spinner_end*4;

	GLfloat yi = y; 
	glBegin(GL_TRIANGLE_STRIP);
	if(spinner_end < n_glyphs)
	{
		yi = y - spinner_end*size;

		GLfloat s = (1.0f/32.0f) * end_glyphs;
		glColor4f(0.0f, 1.0f, 0.0f, 0.3f);
		glTexCoord2f(s, 1.0f); 			glVertex3f(x,yi,z);
		glTexCoord2f(s+1.0f/32.0f, 1.0f);	glVertex3f(x+size,yi,z);
		glTexCoord2f(s, 0.0f); 			glVertex3f(x,yi-size,z);
		glTexCoord2f(s+1.0f/32.0f, 0.0f);	glVertex3f(x+size,yi-size,z);
	}
	glEnd();


/*	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	GLfloat yi = y;
	glBegin(GL_TRIANGLE_STRIP);



	float xi = 0.0f;
	float zi = 0.0f;

	float inc_phi = ph;
	
	//size = 1;


	float phi = 0.0f;
	for(unsigned int i=0; i<spinner_end; i++)
	{
		inc_phi *= r;
		phi += inc_phi;

		float x_next = xi + size * sinf(phi);
		float z_next = zi + size * cosf(phi);
		GLfloat a = 1.0 - cosf(float((int(wave_y - i)%80))*(3.1415926f/180.0f));

		GLfloat s = (1.0f/32.0f) * glyphs[i];
		
		glColor4f(0.0f, 1.0f, 0.0f, a);
		glTexCoord2f(s, 0.0f); 			glVertex3f(xi,yi,zi);	
		glTexCoord2f(s, 0.0f); 			glVertex3f(xi,yi,zi);	
		glTexCoord2f(s, 0.0f); 			glVertex3f(xi,yi,zi);	
		glTexCoord2f(s+1.0f/32.0f, 0.0f);	glVertex3f(x_next,yi,z_next);
		glTexCoord2f(s, 1.0f); 			glVertex3f(xi,yi+size,zi);
		glTexCoord2f(s+1.0f/32.0f, 1.0f);	glVertex3f(x_next,yi+size,z_next);

		xi = x_next;
		zi = z_next;
	}
	glEnd();
*/

/*	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
		
	glBegin(GL_LINE_STRIP);

	float radius = 3.0f;
	float heignt = 15.0f;
	float num = 600.0f;

//	glVertex3f(0.0f, num, 0.0f);
//	glVertex3f(0.0f, 0.0f, 0.0f);
	for(float i=0; i<num; i++)
	{
		float p = f(i/num) * radius;
		float s = sigm((i/num),25.0f);
		float t = sigm((i/num),6.0f);		
		float r = phi(t, 20);
		float x = p * sinf(r);
		float y = s * heignt;
		float z = p * cosf(r);
	
	//	glVertex3f(p, i, 0.0f);
		glVertex3f(x, y, z);
	}
	glEnd();*/
}

float f(float x){ return x*x*21.f - x*19.f + 4.5f; }
float phi(float x, float n)
{
	return x*n*2*3.141926585f;
}
float sigm(float x, float a)
{
	return 1.0f /(1.0f + exp(-a * (x-0.5)));
}

void Matrix::Strip::tick(unsigned long usec, bool update)
{
	double delta  = double(usec)/1000000.0;
	wave += wave_speed * delta;


	ph +=  delta * 0.05f;
	r += delta * 0.001;

	//angle += delta * -90.1f;

//	spinner_end = n_glyphs;
	if(spinner_end < n_glyphs)
	{
		spinner_end += spinner_speed * delta;
	}

	if(spinner_end > n_glyphs)
	{
		spinner_end = n_glyphs;
	}

	if(update)
	{
		end_glyphs = rand()%32;

		for(unsigned int i=0; i<spinner_end*4; i+= 4)
		{
			GLfloat a = 1.0 - cosf(float((int(wave - (i/4))%80))*(3.1415926f/180.0f));

			array[i+0].a = a;
			array[i+1].a = a;
			array[i+2].a = a;
			array[i+3].a = a;

			if(a < 0.000001 )
			{
				GLfloat sa = float(rand()%32)/32.0f;
				GLfloat sb = sa+1.0f/32.0f;

				array[i].s = sa;
				array[i].t = 1.0f;

				array[i+1].s = sb;
				array[i+1].t = 1.0f;

				array[i+2].s = sa;
				array[i+2].t = 0.0f;

				array[i+3].s = sb;
				array[i+3].t = 0.0f;
			}
		}
	}
}

MatrixVideo::MatrixVideo(unsigned int ns, unsigned int ng, TextureAtlas::Texture* texture):Matrix(ns, ng, texture), video_st(NULL), video(NULL)
{
	video_st = new T2F[ns * ng * 4];	// Vertex array for video texture unit
}

MatrixVideo::~MatrixVideo()
{
	delete[] video_st;
}

void MatrixVideo::set_video(const VideoBuffer* buffer, int widht, int height)
{
	video = buffer;

	float video_res[4];
	video_res[0] = widht/10.0;
	video_res[1] = height/10.0;
	video_res[2] = video_res[0]/buffer->s;
	video_res[3] = video_res[1]/buffer->t;

	const unsigned int n = nstrips * nglyphs * 4;
	for(unsigned int i=0; i<n; i++)
	{
		video_st[i].s = (video_res[0] - interleaved_arrays[i].x)/video_res[2];
		video_st[i].t = -interleaved_arrays[i].y/video_res[3];
	}
}
	
void MatrixVideo::pre_draw()
{
//	Video & fixed pipeline
	Matrix::pre_draw();

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	video->bind(GL_MODULATE);

	glClientActiveTexture(GL_TEXTURE1);
	glTexCoordPointer(2, GL_FLOAT, 0, &video_st[0].st);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void MatrixVideo::post_draw()
{
	Matrix::post_draw();
}

MatrixVideoFX::MatrixVideoFX(unsigned int ns, unsigned int ng, TextureAtlas::Texture* texture):MatrixVideo(ns, ng, texture), program(NULL)
{
	Shader vshader(Shader::Vertex);

	const GLchar* vertex_shader = 
	"uniform sampler2D video;"
	"void main(void)"
	"{"
	" vec4 color = texture2D(video, gl_MultiTexCoord1.st);"
	" vec4 vertex = gl_Vertex;"
	" vertex.z = color.r*1.4;"
	" gl_FrontColor = gl_Color;"
	" gl_TexCoord[0] = gl_MultiTexCoord0;"
	" gl_TexCoord[1] = gl_MultiTexCoord1;"
	" gl_Position = gl_ModelViewProjectionMatrix * vertex;"
	"}";

	vshader.set_source(vertex_shader);
	vshader.compile();
	vshader.log();

	program = new GPU_Program;

	program->attach(vshader);
	program->link();
	program->validate();
	program->log();
}

MatrixVideoFX::~MatrixVideoFX()
{
	delete program;
}
	
void MatrixVideoFX::pre_draw()
{
//	Video & Vertex shader
	MatrixVideo::pre_draw();

	program->use();		// Enable shader
	program->set_sampler("video", 1);
}

void MatrixVideoFX::post_draw()
{
	GPU_Program::use_default();	// Disable shader, use fixed pipeline

	MatrixVideo::pre_draw();
}

float Scene::user_a = 0.0f;
float Scene::user_b = 0.0f;

Scene::Scene(class AppWindow* win, bool capturing_enable):GLView(win), atlas(1), screen(NULL), matrix(NULL)
{
	if(capturing_enable)
	{
		if( Options::get("--no-shaders") || version() <= "2.0")
		{
			matrix = new MatrixVideo(128, 112, atlas[0]);
		}
		else
		{
			matrix = new MatrixVideoFX(128, 112, atlas[0]);
		}
	}
	else matrix = new Matrix(128, 112, atlas[0]);
}

Scene::~Scene()
{
	delete matrix;
}

unsigned int Scene::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	matrix->draw();
	return 0;
}

unsigned int Scene::tick(unsigned long usec)
{	
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
				user_a += 0.05f;
			break;

			case 104:	// down
				user_a -= 0.05f;
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

