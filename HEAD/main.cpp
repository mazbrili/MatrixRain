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
	{ "--root",  	"false"		},
	{ "--width",	"600"		},
  	{ "--height",	"480"		},
	{ "--device",	"/dev/video0"	},
	{ "--convert",	"false"		},
	{ NULL, 	NULL		},
};

//FILE* log22 = NULL;

int main(int argc, char **argv)
{
	Options::load(argc, argv, default_opts);

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

