//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	texture_array.h
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef TEXTURE_ARRAY_H
#define TEXTURE_ARRAY_H
//--------------------------------------------------------------
#include <cstdio>
#include <cstdlib>

#include <GL/gl.h>

#include "capture.h"
#include "stuff.h"
//--------------------------------------------------------------
class TextureArray
{
protected:
	class Texture
	{
	public:
		Texture(GLuint arg_id, GLenum t):id(arg_id), type(t){}
		virtual ~Texture(){}

		inline void bind(){ glBindTexture (type, id); }
	private:
		
		GLuint id;
		GLenum type;
	};

	TextureArray(unsigned int n);
	~TextureArray();

	Texture** textures;
	GLuint* texture_id;
	unsigned int num_textures;
};
//--------------------------------------------------------------
#endif//TEXTURE_ARRAY_H
//--------------------------------------------------------------

