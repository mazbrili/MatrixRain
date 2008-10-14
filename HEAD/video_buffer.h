//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	video_buffer.h
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef VIDEO_BUFFER_H
#define VIDEO_BUFFER_H
//--------------------------------------------------------------
#include "texture_array.h"
#include "bitmap.h"
//--------------------------------------------------------------
class VideoBuffer:public TextureArray
{
public:
	class VideoFrame:public TextureArray::Texture
	{
	public:
		VideoFrame(char* data, unsigned int width, unsigned int height, GLuint tex_id);
		void update(char* data, unsigned int offset_x, unsigned int offset_y, unsigned int width, unsigned int height);
	private:
	};

	VideoBuffer(const Bitmap& frame, int sub_layers, unsigned long micsec_delay);

	void operator()(const Bitmap& frame, unsigned long time);
	virtual void bind(GLint texture_function)const;

	float s;
	float t;

protected:
	unsigned int top_index;
private:
	unsigned long last_time;
	unsigned long delay;
};
//--------------------------------------------------------------
#endif//VIDEO_BUFFER_H
//--------------------------------------------------------------
