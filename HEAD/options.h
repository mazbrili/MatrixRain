//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	options.h
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef OPTIONS_H
#define OPTIONS_H
//--------------------------------------------------------------
#include <string>
//--------------------------------------------------------------
class Options
{
public:
	struct Option
	{
		operator std::string()const;
		operator const char*const()const;
		operator bool ()const;
		operator int ()const;
		operator float ()const;
		operator double ()const;

		const char*	name;
		const char*	value;
	};

	static bool load(int argc, char **argv, Option* defaults);
	static const Option& get(const char* name);

private:
	static Option* options;
};
//--------------------------------------------------------------
#endif//OPTIONS_H
//--------------------------------------------------------------

