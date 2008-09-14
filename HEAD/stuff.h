//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	stuff.h
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef STUFF_H
#define STUFF_H
//--------------------------------------------------------------
#include <stdexcept>
#include <sys/time.h>
//--------------------------------------------------------------
class Random
{
	enum { table_size=55 };
public:
	enum { random_max=0xFFFFFFFF };
	Random(unsigned int seed=0);
	

	unsigned int operator()();
	float operator()(float f);
private:
	int index1, index2;

	static unsigned int table[table_size];
};


extern Random grnd;



class Timer
{
public:
	Timer();

	void reset();
	unsigned long time();
	unsigned long tick();
private:
	struct timeval prev;
	struct timeval begin;
};

bool convert_bmp_2_include_array(char* bmp_file, char* array_name);

inline unsigned int round_pow_2(unsigned int a)
{
	--a;
	a |= a >> 1;
	a |= a >> 2;
	a |= a >> 4;
	a |= a >> 8;
	a |= a >> 16;
	return ++a;
}
//--------------------------------------------------------------
#endif//STUFF_H
//--------------------------------------------------------------

