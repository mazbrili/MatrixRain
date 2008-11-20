//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	matrix.cpp
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#include "matrix.h"
//--------------------------------------------------------------
Matrix::Matrix(unsigned int ns, unsigned int ng, TextureAtlas::Texture* texture): letter(texture),
	interleaved_arrays(NULL), firsts(NULL), counts(NULL),
	nstrips(ns), nglyphs(ng),
	strips(NULL), grid_random(324467), animation_period(20000000)
{
	unsigned int strip_pack = nglyphs * 4;
	interleaved_arrays = new GLView::T2F_V3F_C4F[nstrips * strip_pack];
	firsts = new GLint[nstrips];
	counts = new GLsizei[nstrips];

	strips = new Strip*[nstrips];

	for(unsigned int i=0; i<nstrips; i++)
	{
		strips[i] = new Strip(	strip_pack, &interleaved_arrays[i*strip_pack], 
					float(i)/2.0f, -grid_random(3.0f), 0, grid_random(5.0f)+5.0f, grid_random(5.0f)+5.0f,
					-48.0f, 35.0f, grid_random(5.0f)+2.0f, grid_random(10.0f)+15.0f, grid_random(1.0f)+5.0f, grid_random(10.0f)+10.0f);
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
//	glLoadIdentity();
//	glTranslatef(-32.0,24.0,-25.0f);

	pre_draw();
	for(unsigned int i=0; i<nstrips; i++)
	{
		strips[i]->draw(&firsts[i], &counts[i]);
		firsts[i] += (i * nglyphs<<2);
	}

	glMultiDrawArrays(GL_TRIANGLE_STRIP, firsts, counts, nstrips);		
	post_draw();


//	strips[0]->draw(&firsts[0], &counts[0]);

/*	for(unsigned int i=0; i<nstrips; i++)
	{
		strips[i]->draw(&firsts[i], &counts[i]);
	}
*/
}

void Matrix::tick(unsigned long usec)
{	
	if( animation_period.ready() )
	{
		for(unsigned int i=0; i<nstrips; i++)
		{
			if( !strips[i]->arunning )
			{
				strips[i]->arunning = true;
				unsigned int delay = abs(i-nstrips/2) * 120000;
				strips[i]->adelay = delay + grandom(30000U);// - grandom(i * 100000U);
			}
		}
	}

	for(unsigned int i=0; i<nstrips; i++)
	{
		strips[i]->tick(usec);
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
	
//	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);

	glActiveTexture(GL_TEXTURE0);	
	glEnable(GL_TEXTURE_2D);
	letter->bind();
	glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_MODULATE);

	glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(2, GL_FLOAT, sizeof(GLView::T2F_V3F_C4F), &interleaved_arrays[0].st);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glColorPointer	 (4, GL_FLOAT, sizeof(GLView::T2F_V3F_C4F), &interleaved_arrays[0].rgba);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer  (3, GL_FLOAT, sizeof(GLView::T2F_V3F_C4F), &interleaved_arrays[0].xyz);
	glEnableClientState(GL_VERTEX_ARRAY);
}

void Matrix::post_draw()
{
//	glDepthMask(GL_TRUE);
	glPopClientAttrib();	// Load client state	
	glPopAttrib();		// Load server state
}

Matrix::Strip::Strip(unsigned int n, GLView::T2F_V3F_C4F* interleaved_arrays, GLfloat x, GLfloat y, GLfloat z, GLfloat, GLfloat, float h1, float h2, float r, float p, float q, float rotates):array(interleaved_arrays), animation(2048, vector(x, y, z), vector(32.0f, -35.0f, -35.0f), h1, h2, r, p, q, rotates), wave_waiter(100000 + grandom(40000U)), aframe_waiter(10000), n_glyphs(n/4), end_glyph(0), wavehead(0), adelay(0), aframe(0), arunning(false)
{
	size = 0.4f;

	for(unsigned int i=0; i<n; i+= 4)
	{
		GLfloat a = 1.0f - cosf(float((int(wavehead - (i/4))%80))*(3.1415926f/180.0f));

		GLfloat s = float(rand()%32)/32.0f;
		
		array[i].s = s;
		array[i].t = 1.0f;
		array[i].r = 0.0f;
		array[i].g = 1.0f;
		array[i].b = 0.0f;
		array[i].a = a;
		
		array[i+1].s = s+1.0f/32.0f;
		array[i+1].t = 1.0f;
		array[i+1].r = 0.0f;
		array[i+1].g = 1.0f;
		array[i+1].b = 0.0f;
		array[i+1].a = a;

		array[i+2].s = s;
		array[i+2].t = 0.0f;
		array[i+2].r = 0.0f;
		array[i+2].g = 1.0f;
		array[i+2].b = 0.0f;
		array[i+2].a = a;
				
		array[i+3].s = s+1.0f/32.0f;
		array[i+3].t = 0.0f;
		array[i+3].r = 0.0f;
		array[i+3].g = 1.0f;
		array[i+3].b = 0.0f;
		array[i+3].a = a;
	}
	
	GLfloat yi = y;
	for(unsigned int i=0; i<n; i+= 4)
	{
		array[i].x = x;
		array[i].y = yi;
		array[i].z = z;
		
		array[i+1].x = x+size;
		array[i+1].y = yi;
		array[i+1].z = z;
	
		array[i+2].x = x;
		array[i+2].y = yi-size;
		array[i+2].z = z;
				
		array[i+3].x = x+size;
		array[i+3].y = yi-size;
		array[i+3].z = z;

		yi -= size;
	}
/*
	for(unsigned int i=0, j=aframe; i<n; i+= 4, j++)
	{
		vector v0 = animation[j];
		vector v1 = v0;
		v1.x += size;
		vector v2 = animation[j+1];
		vector v3 = v2;
		v3.x += size;

		array[i].x = v0.x;
		array[i].y = v0.y;
		array[i].z = v0.z;
		
		array[i+1].x = v1.x;
		array[i+1].y = v1.y;
		array[i+1].z = v1.z;
	
		array[i+2].x = v2.x;
		array[i+2].y = v2.y;
		array[i+2].z = v2.z;
				
		array[i+3].x = v3.x;
		array[i+3].y = v3.y;
		array[i+3].z = v3.z;
	}*/
}

Matrix::Strip::~Strip()
{
}

void Matrix::Strip::draw(GLint* first, GLsizei* count)
{
	*first = 0;
	*count = end_glyph*4;
/*
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	glBegin(GL_LINE_STRIP);
	for(int i=0; i<2048; i++)
	{
		vector v = animation[i];
		glVertex3fv(v.xyz);
	}
	glEnd();
*/
}

void Matrix::Strip::tick(unsigned long usec)
{
	if( unsigned int inc = wave_waiter.test(usec) )
	{
	//	end_glyph = n_glyphs;
		if(end_glyph < n_glyphs && (aframe+end_glyph) < animation.nframes )
		{
			end_glyph += inc;
		}

		wavehead += inc;

		unsigned int i= (wavehead%end_glyph) * 4;

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



		for(unsigned int i=0; i<end_glyph*4; i+= 4)
		{
			GLfloat a = 1.0f - cosf(float((int((wavehead - (i/4)))%80))*(3.1415926f/180.0f));

			array[i+0].a = a;
			array[i+1].a = a;
			array[i+2].a = a;
			array[i+3].a = a;
		}
	}

	if(arunning)
	{
		if(adelay > 0)
		{
			adelay -= usec;
		}
		else
		{
			if(unsigned int iframe = aframe_waiter.test(usec))
			{
				aframe += iframe;
				if( aframe >= animation.nframes ) 	// animation ends
				{
					aframe = 0;
					arunning = false;	// stop animation
				}

				bool b = false;
			//	end_glyph = n_glyphs;
				for(unsigned int i=0, j=aframe; i<n_glyphs*4; i+= 4, j++)
				{
					if( (j+1) == animation.nframes )
					{
						b = true;
					//	--end_glyph;
					//	break;
					}
					vector v0 = animation[j];
					vector v1 = v0;
					v1.x += size;
					vector v2 = animation[j+1];
					vector v3 = v2;
					v3.x += size;

					array[i].x = v0.x;
					array[i].y = v0.y;
					array[i].z = v0.z;
		
					array[i+1].x = v1.x;
					array[i+1].y = v1.y;
					array[i+1].z = v1.z;
	
					array[i+2].x = v2.x;
					array[i+2].y = v2.y;
					array[i+2].z = v2.z;
				
					array[i+3].x = v3.x;
					array[i+3].y = v3.y;
					array[i+3].z = v3.z;
				}

				if( b && end_glyph > 0 )
				{
					end_glyph = 0;
				}

			}
		}
	}
}

MatrixVideo::MatrixVideo(unsigned int ns, unsigned int ng, TextureAtlas::Texture* texture):Matrix(ns, ng, texture), video_st(NULL), video(NULL)
{
	video_st = new GLView::T2F[ns * ng * 4];	// Vertex array for video texture unit
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
	" vertex.z += color.r*1.4;"
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
//--------------------------------------------------------------

