//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	main.cpp
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#include <cctype>
#include "options.h"
#include "stuff.h"
#include "application.h"
//--------------------------------------------------------------
Options::Option default_opts[]={
	{ "--help",	"false",	"show this message"				},
	{ "--root",  	"false",	"setup a root window for fullscreen mode"	},
	{ "--window-id","0",		"specific X window ID"				},
	{ "--width",	"600",		"setup a width of screensaver window"		},
  	{ "--height",	"480",		"setup a height of screensaver window"		},
	{ "--device",	"/dev/video0",	"setup a device for capturing video"		},
	{ "--convert",	"false",	"my little bitmap converter. usage: mrain --convert <input.bmp> <output.filename>" },
	{ NULL, 	NULL,		NULL	},
};

int main(int argc, char **argv)
{
	Options::load(argc, argv, default_opts);


	if( Options::get("--help") ) return Options::usage();

	// A little converter bmp -> *.h	
	if( Options::get("--convert") )
	{
		return convert_bmp_2_include_array(argv[2], argv[3]) == false;
	}


//	log22 = fopen("/home/sqrt/mrain.log", "ab");
//	fprintf(log22,"%s %d", __FILE__, __LINE__);
//	fflush(log22);

	Application application;

	return application.run();
}
//--------------------------------------------------------------

