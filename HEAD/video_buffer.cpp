//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	video_buffer.cpp
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#include "video_buffer.h"
//--------------------------------------------------------------
VideoBuffer::VideoFrame::VideoFrame(char* data, unsigned int width, unsigned int height, GLuint tex_id):TextureArray::Texture(tex_id, GL_TEXTURE_2D)
{
	bind();

	unsigned int tex_width = round_pow_2(  width  );
	unsigned int tex_height = round_pow_2( height );

	char dummy[tex_width*tex_height];
	memset (dummy, 0, sizeof (dummy));

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);						
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
//	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 16);
//	glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
//	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0.0f);
//	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 4.0f);
//	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -1.0f);
//	glTexParameteri (GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_REPLACE);
	
	glTexImage2D (GL_TEXTURE_2D, 0, GL_LUMINANCE8, tex_width, tex_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, dummy);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
}

void VideoBuffer::VideoFrame::update(char* data, unsigned int offset_x, unsigned int offset_y, unsigned int width, unsigned int height)
{
	glTexSubImage2D(GL_TEXTURE_2D, 0, offset_x, offset_y, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
}

VideoBuffer::VideoBuffer(const Bitmap& frame, int layers, unsigned long micsec_delay):TextureArray(layers+1 /*for current*/), top_index(0), last_time(0), delay(micsec_delay)
{
	s = float(frame.width())  / float(round_pow_2( frame.width()  ));
	t = float(frame.height()) / float(round_pow_2( frame.height() ));

	for(unsigned int i=0; i<num_textures; i++)
	{
		textures[i] = new VideoFrame(frame.data(), frame.width(), frame.height(), texture_id[i]);
	}
}

void VideoBuffer::operator()(const Bitmap& frame, unsigned long time)
{
	if(time - last_time >= delay)
	{
		top_index = (++top_index) % num_textures;
		last_time = time;
	}

	VideoFrame* video = (VideoFrame*)textures[top_index];
	video->bind();
	video->update(frame.data(), 0, 0, frame.width(), frame.height());
}

void VideoBuffer::bind()const
{
	textures[top_index]->bind();
}
//--------------------------------------------------------------

