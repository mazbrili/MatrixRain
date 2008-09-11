//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	stuff.cpp
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#include <sys/time.h> /* for gettimeofday() */
#include <cstdarg>
#include "stuff.h"
//--------------------------------------------------------------
// The following 'random' numbers are taken from CRC, 18th Edition, page 622.

Random grnd;

unsigned int Random::table[Random::table_size] = {
 035340171546, 010401501101, 022364657325, 024130436022, 002167303062, /*  5 */
 037570375137, 037210607110, 016272055420, 023011770546, 017143426366, /* 10 */
 014753657433, 021657231332, 023553406142, 004236526362, 010365611275, /* 14 */
 007117336710, 011051276551, 002362132524, 001011540233, 012162531646, /* 20 */
 007056762337, 006631245521, 014164542224, 032633236305, 023342700176, /* 25 */
 002433062234, 015257225043, 026762051606, 000742573230, 005366042132, /* 30 */
 012126416411, 000520471171, 000725646277, 020116577576, 025765742604, /* 35 */
 007633473735, 015674255275, 017555634041, 006503154145, 021576344247, /* 40 */
 014577627653, 002707523333, 034146376720, 030060227734, 013765414060, /* 45 */
 036072251540, 007255221037, 024364674123, 006200353166, 010126373326, /* 50 */
 015664104320, 016401041535, 016215305520, 033115351014, 017411670323  /* 55 */
};

Random::Random(unsigned int seed)
{
	if (0 == seed)
	{
		struct timeval tp;
		#ifdef GETTIMEOFDAY_TWO_ARGS
			struct timezone tzp;
			gettimeofday(&tp, &tzp);
		#else
			gettimeofday(&tp);
		#endif
		/* ignore overflow */
		seed = (999*tp.tv_sec) + (1001*tp.tv_usec) + (1003 * getpid());
	}

	table[0] += seed;
	for (int i=1; i<table_size; i++)
	{
		seed = table[i-1]*1001 + seed*999;
		table[i] += seed;
	}

	index1 = table[0] % table_size;
	index2 = (index1 + 024) % table_size;
}

unsigned int Random::operator()()
{
	register int r = table[index1] + table[index2];
	table[index1] = r;
	if (++index1 >= table_size) index1 = 0;
	if (++index2 >= table_size) index2 = 0;
	return r;
}

float Random::operator()(float f)
{
	double tmp = ( double((*this)()) * double(f)) / double(random_max);
	return (tmp < 0) ? -tmp : tmp;
}

Timer::Timer()
{
	reset();
}

void Timer::reset()
{
	gettimeofday(&begin, NULL);
	prev = begin;
}

unsigned long Timer::time()
{
	struct timeval now;
	gettimeofday(&now, NULL);

	return (now.tv_sec - begin.tv_sec) * 1000000 - begin.tv_usec + now.tv_usec;
}

unsigned long Timer::tick()
{
	struct timeval now;
	gettimeofday(&now, NULL);

	unsigned long delta = (now.tv_sec - prev.tv_sec) * 1000000 - prev.tv_usec + now.tv_usec;
	prev = now;

	return delta;
}

bool convert_bmp_2_include_array(char* bmp_file, char* array_name)
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
		unsigned long   biWidth; 
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


	FILE* src = fopen(bmp_file,"rb");
	fprintf(stdout,"open %s: %s\n", bmp_file, src ? "successed" : "failed");
	if(NULL == src) return false;
	
	char out_name[256]={'\0'};
	sprintf(out_name, "%s.h", array_name);
	FILE* dst = fopen(out_name,"wb");
	fprintf(stdout,"create %s: %s\n", out_name, dst ? "successed" : "failed");
	if(NULL == dst) return false;
	
	char* p = out_name;
	while(char c = *p)
	{
		if( isalnum(c) )
		{
			*p = toupper(c);
		}
		else *p = '_';
			p++;
	}

	FILEHEADER file_header;
	fread(&file_header, sizeof(FILEHEADER), 1, src );
	if(file_header.bfType[0] != 'B' || file_header.bfType[1] != 'M' )
		return false;

	INFOHEADER info_header;
	fread(&info_header, sizeof(INFOHEADER), 1, src );

	unsigned long width = info_header.biWidth;
	unsigned long height = info_header.biHeight;
	unsigned int size = width * height * (info_header.biBitCount/8);
	
	fprintf(dst,"#ifndef %s\n#define %s\n", out_name, out_name);
	fprintf(dst,"const unsigned %s_width=%ld;\n",array_name, width);
	fprintf(dst,"const unsigned int %s_height=%ld;\n",array_name,  height);
	fprintf(dst,"const unsigned char %s[]={\n", array_name);
	unsigned char buffer[16];
	for(unsigned int i=0; i<(size/16); i++)
	{
		int count = fread(buffer, sizeof(char), sizeof(buffer), src);

		for(int k=0; k<count; k+=4)
		{		
			unsigned int b = buffer[k+3];
			if(b == 0)
			{
				fprintf(dst,"0x0, 0xFF, 0x0, 0x0, ");
			}
			else	fprintf(dst,"0x0, 0x0, 0x0, 0xFF, ");
		}
		fprintf(dst,"\n");
	}

	fprintf(dst,"};\n#endif//%s\n\n", out_name);

	fclose(src);
	fclose(dst);
	return true;
}

//--------------------------------------------------------------

