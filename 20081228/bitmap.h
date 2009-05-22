//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	bitmap.h
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef BITMAP_H
#define BITMAP_H
//--------------------------------------------------------------
#include <cstdio>

#include "capture.h"
//--------------------------------------------------------------
class Bitmap
{
public:
	Bitmap():image_width(0), image_height(0), buffer(NULL){ }

	~Bitmap(){ clear(); }

	unsigned int width()const { return image_width;  }
	unsigned int height()const{ return image_height; }
	char* data()const { return buffer; }

	Bitmap& operator << (Capture& capture)
	{
		clear();

		image_width = capture.width();
		image_height = capture.height();

		Capture::out_format format = Capture::GRAY;

		size_t size = image_width*image_height*Capture::bpp(format);
		buffer = new char [size];
		capture.set_buffer(buffer, format);

		return *this;
	}

	bool dump(const char* bmp_file)const
	{
		struct __attribute__ ((__packed__)) FILEHEADER
		{
			unsigned char	bfType[2];
			unsigned int	bfSize;
			unsigned short	bfReserved1;
			unsigned short	bfReserved2;
			unsigned int	bfOffBits;
		};

		typedef struct tagINFOHEADER
		{
			unsigned int	biSize; 
			unsigned long	biWidth; 
			unsigned long	biHeight; 
			unsigned short	biPlanes; 
			unsigned short	biBitCount; 
			unsigned int	biCompression; 
			unsigned int	biSizeImage; 
			unsigned long	biXPelsPerMeter; 
			unsigned long	biYPelsPerMeter; 
			unsigned int	biClrUsed; 
			unsigned int	biClrImportant; 
		} INFOHEADER, *PINFOHEADER;
	
		fprintf (stderr, "dump bitmap into %s\n", bmp_file);

		unsigned int dob_width = (4 - (image_width & 0x3)) & 0x3; 
		unsigned int rounded_width = image_width + dob_width; 
		unsigned int data_size = rounded_width*image_height*3;	// bpp == 24

		FILEHEADER file;
		INFOHEADER info;

		file.bfType[0] = 'B';
		file.bfType[1] = 'M';
		file.bfSize = sizeof(file) + sizeof(info) + data_size;
		file.bfReserved1 = 0;
		file.bfReserved2 = 0;
		file.bfOffBits = 0;

		info.biSize = sizeof(INFOHEADER); 
		info.biWidth = image_width;
		info.biHeight = image_height;
		info.biPlanes = 1; 
		info.biBitCount = 24; 
		info.biCompression =0; 
		info.biSizeImage = 0; 
		info.biXPelsPerMeter = 1; 
		info. biYPelsPerMeter = 1; 
		info.biClrUsed = 0; 
		info.biClrImportant = 0;

		size_t res = 0;

		FILE* mfd = fopen(bmp_file, "wb");
		res = fwrite(&file, sizeof(file), 1, mfd);
		res = fwrite(&info, sizeof(info), 1, mfd);

		char* pixel = buffer;
		for(unsigned int i=0; i<image_height; i++)
		{
			for(unsigned int j=0; j<image_width; j++)
			{
				char bmp_pixel[3]={pixel[0],pixel[1],pixel[2]};
				res = fwrite(bmp_pixel, sizeof(bmp_pixel), 1, mfd);
			//	pixel++;
				pixel+=3;
			}
			char dummy[dob_width];
			res = fwrite(dummy, sizeof(dummy), 1, mfd);
		}

		fclose(mfd);

		return true;
	}

private:

	inline void clear()
	{
		delete[] buffer;
		buffer = NULL;
	}
	
	unsigned int image_width;
	unsigned int image_height;
	char* buffer;
};
//--------------------------------------------------------------
#endif//BITMAP_H
//--------------------------------------------------------------
