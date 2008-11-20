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
	unsigned int operator()(unsigned int i);
private:
	int index1, index2;

	static unsigned int table[table_size];
};

extern Random grandom;

class Timer
{
	friend class Waiter;
public:
	Timer();

	void reset();
	unsigned long time();
	unsigned long tick();
private:
	struct timeval prev;
	struct timeval begin;
};

class Counter
{
public:
	explicit Counter(unsigned int l):limit(l), count(0U){}

	inline unsigned int test(unsigned int delta)
	{	// integer calculations !
		// Wirning ! There is an owerflow !
		count += delta;
		unsigned int uptimes = count/limit;
		count %= limit;
		return uptimes;
	}
private:
	unsigned int limit;
	unsigned int count;
};

class Waiter
{
	static Timer timer;
public:
	Waiter(unsigned long microseconds);

	bool ready();
	static void tick();
private:
	unsigned long sec;
	unsigned long usec;
	struct timeval end;
};



class Version
{
public:
	Version(const char* string);
	Version(unsigned char major=0, unsigned char minor=0, unsigned char release=0);
	Version(const Version& v):iversion(v.iversion){}

	Version& operator=(const Version& v){ iversion = v.iversion; return *this; }
	Version& operator=(const char* str);

	inline bool operator>=(Version v)const{ return iversion >= v.iversion; }
	inline bool operator<=(Version v)const{ return iversion <= v.iversion; }
	inline bool operator==(Version v)const{ return iversion == v.iversion; }

	bool operator>=(const char* str);


private:
	unsigned int iversion; 
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

