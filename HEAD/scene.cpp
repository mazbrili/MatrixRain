//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	scene.cpp
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#include <cmath>

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
	
		video->bind();
	}
	glBegin(GL_QUADS);
		glTexCoord2f(video->s, video->t); glVertex3f(0.0, 0.0, 0.0);
		glTexCoord2f(video->s, 0.0f);	glVertex3f(0.0, height, Scene::user_b);
		glTexCoord2f(0.0f, 0.0f);	glVertex3f(width, height, Scene::user_b);
		glTexCoord2f(0.0f, video->t);	glVertex3f(width, 0.0, 0.0);
	glEnd();
}

void VideoScreen::tick(unsigned long usec)
{
}

Matrix::Matrix(unsigned int n, int widht, int height):video(NULL), program(NULL), num(n), strips(NULL), grid_random(324467)
{
	video_res[0] = widht/10.0;
	video_res[1] = height/10.0;
	video_res[2] = 0;
	video_res[3] = 0;

	strips = new Strip*[num];
	for(unsigned int i=0; i<num; i++)
	{
		strips[i] = new Strip(112, float(i)/2.0f, -grid_random(3.0f), 0, grid_random(5.0f)+5.0f, grid_random(5.0f)+5.0f);
	}


	Shader vshader(Shader::Vertex);

	const GLchar* vertex_shader = 
	"uniform vec4 res;"
	"uniform sampler2D video;"
	"void main(void)"
	"{"
	" vec2 st = vec2( (res[0] - gl_Vertex.x)/res[2], -gl_Vertex.y/res[3] );"
	" vec4 color = texture2D(video, st);"
	" vec4 vertex = gl_Vertex;"
	" vertex.z = color.r*1.4;"
	" gl_FrontColor = gl_Color;"
	" gl_TexCoord[0] = gl_MultiTexCoord0;"
	" gl_TexCoord[1].st = st;"
	" gl_Position = gl_ModelViewProjectionMatrix * vertex;"
	"}";

	vshader.set_source(vertex_shader);
	vshader.compile();
	vshader.log();

	Shader fshader(Shader::Fragment);

	const GLchar* fragment_shader = 
	"uniform sampler2D letter;"
	"uniform sampler2D video;"
	"void main(void)"
	"{"
	" vec4 texture = texture2D(letter, gl_TexCoord[0].st);"
	" if(texture.a == 1.0) discard;"
	" vec4 vid = texture2D(video, gl_TexCoord[1].st);"
	" gl_FragColor = texture * vid.r * (1.0 - gl_Color.a);"
//	" gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);"
	"}";

	fshader.set_source(fragment_shader);
	fshader.compile();
	fshader.log();

	program = new GPU_Program;

	program->attach(vshader);
	program->attach(fshader);
	program->link();
	program->log();
}

Matrix::~Matrix()
{
	delete program;
	for(unsigned int i=0; i<num; i++)
	{
		delete strips[i];
	}
	delete[] strips;
}

void Matrix::set_video(const VideoBuffer* buffer)
{
	video = buffer;

	video_res[2] = video_res[0]/buffer->s;
	video_res[3] = video_res[1]/buffer->t;
}

void Matrix::draw()
{	
	glLoadIdentity();
//	glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
	glTranslatef(-32.0,24.0,-25.0f);

//	glTranslatef(0,0,-10.0f);
//	glRotatef(30.0f, 1.0f, 0.0f, 0.0f);

	if( video )
	{
		program->use();

		program->set_sampler("letter",0);
		program->set_sampler("video", 1);
		program->set_uniform("res", video_res);

		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);
		video->bind();	
	}
	else glEnable(GL_BLEND);
	
	glActiveTexture(GL_TEXTURE0);		
	glEnable(GL_TEXTURE_2D);
	letter->bind();

	for(unsigned int i=0; i<num; i++)
	{			
		strips[i]->draw();
	}

//	strips[0]->draw();
}

void Matrix::tick(unsigned long usec)
{
	bool update = false;
	glyph_update.tick();

	if(300000 < glyph_update.time() )
	{
		update = true;
		glyph_update.reset();	
	}

	for(unsigned int i=0; i<num; i++)
	{
		strips[i]->tick(usec, update);
	}
}

Matrix::Strip::Strip(unsigned int n, GLfloat ax, GLfloat ay, GLfloat az, GLfloat spinner, GLfloat wave):x(ax), y(ay), z(az), n_glyphs(n)
{
	size = 0.4f;

	spinner_speed = spinner;
	spinner_begin = 0.0f;
	spinner_end = 0.0f;

	glyphs = new int[n_glyphs];

	for(unsigned int i=0; i<n_glyphs; i++)
	{
		glyphs[i] = rand()%32;
	}

	wave_y = 0.0f;
	wave_speed = wave;
}

Matrix::Strip::~Strip()
{
	delete[] glyphs; 
}

void Matrix::Strip::draw()
{	
	GLfloat yi = y; 
	glBegin(GL_TRIANGLE_STRIP);
	for(unsigned int i=0; i<spinner_end; i++)
	{
		GLfloat a = 1.0 - cosf(float((int(wave_y - i)%80))*(3.1415926f/180.0f));

		GLfloat s = (1.0f/32.0f) * glyphs[i];
		
		glColor4f(0.0f, 1.0f, 0.0f, a);
		glTexCoord2f(s, 1.0f); 			glVertex3f(x,yi,z);	
		glTexCoord2f(s+1.0f/32.0f, 1.0f);	glVertex3f(x+size,yi,z);
		glTexCoord2f(s, 0.0f); 			glVertex3f(x,yi-size,z);
		glTexCoord2f(s+1.0f/32.0f, 0.0f);	glVertex3f(x+size,yi-size,z);

		yi -= size;
	}

	if(spinner_end < n_glyphs)
	{
		yi = y - spinner_end*size;

		GLfloat s = (1.0f/32.0f) * end_glyphs;
		glColor4f(0.0f, 1.0f, 0.0f, 0.0f);
		glTexCoord2f(s, 1.0f); 			glVertex3f(x,yi,z);
		glTexCoord2f(s+1.0f/32.0f, 1.0f);	glVertex3f(x+size,yi,z);
		glTexCoord2f(s, 0.0f); 			glVertex3f(x,yi-size,z);
		glTexCoord2f(s+1.0f/32.0f, 0.0f);	glVertex3f(x+size,yi-size,z);
	}

	glEnd();

/*	GLfloat yi = y;
	glBegin(GL_TRIANGLE_STRIP);


	float xi = 0.0f;
	float zi = 0.0f;

	float inc_phi = 1.2f;
	float inc_p   = 0.25f;

	size = 1;


	float phi = 0.0f;
	float p   = inc_p;
	float k = 0.2;
	for(unsigned int i=0; i<spinner_end; i++)
	{
		inc_phi *= 0.95f;
		phi += inc_phi;

		float x_next = xi + size * sinf(phi);
		float z_next = zi + size * cosf(phi);
		GLfloat a = 1.0 - cosf(float((int(wave_y - i)%80))*(3.1415926f/180.0f));

		GLfloat s = 0;//(1.0f/32.0f) * glyphs[i];
		
		glColor4f(0.0f, 1.0f, 0.0f, 0.0f);
		glTexCoord2f(s, 0.0f); 			glVertex3f(xi,yi,zi);	
		glTexCoord2f(s, 0.0f); 			glVertex3f(xi,yi,zi);	
		glTexCoord2f(s, 0.0f); 			glVertex3f(xi,yi,zi);	
		glTexCoord2f(s+1.0f/32.0f, 0.0f);	glVertex3f(x_next,yi,z_next);
		glTexCoord2f(s, 1.0f); 			glVertex3f(xi,yi+size,zi);
		glTexCoord2f(s+1.0f/32.0f, 1.0f);	glVertex3f(x_next,yi+size,z_next);

		xi = x_next;
		zi = z_next;
	}
*/
}

void Matrix::Strip::tick(unsigned long usec, bool update)
{
	double delta  = double(usec)/1000000.0;
	wave_y += wave_speed * delta;

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
		end_glyphs = glyphs[rand()%32];
		for(unsigned int i=0; i<spinner_end; i++)
		{
			GLfloat a = 1.0 - cosf(float((int(wave_y - i)%80))*(3.1415926f/180.0f));

			if(a < 0.000001 )
			{
				glyphs[i] = rand()%32;
			}
		}
	}
}

float Scene::user_a = 0.0f;
float Scene::user_b = 0.0f;

Scene::Scene(class AppWindow* win):GLView(win), atlas(1), screen(64.0f, 48.0f), matrix(128, 640, 480)
{
	matrix.letter = atlas[0];
}

Scene::~Scene()
{
}

unsigned int Scene::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
//	screen.draw();
	matrix.draw();
	return 0;
}

unsigned int Scene::tick(unsigned long usec)
{	
//	screen.tick(usec);
	matrix.tick(usec);
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

