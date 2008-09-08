//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	options.cpp
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#include "options.h"
//--------------------------------------------------------------
Options::Option* Options::options = NULL;

Options::Option::operator std::string()const
{
	if(NULL == value)
	{
		fprintf (stderr, "resource not found, return default value: empty-string.\n");
		return std::string();
	}
	std::string str(value);	
	return str;
}

Options::Option::operator const char*const()const
{
	return value;
}

Options::Option::operator bool ()const
{
	if(NULL == value)
	{
		fprintf (stderr, "resource not found, return default value: false.\n");
		return false;
	}

	char buf [100];
	char* tmp = buf;

	for (const char *s = value; *s ; s++)
		*tmp++ = isupper (*s) ? tolower (*s) : *s;
	*tmp = '\0';
	
	if (!strcmp (buf, "on") || !strcmp (buf, "true") || !strcmp (buf, "yes"))
		return true;

	if (!strcmp (buf,"off") || !strcmp (buf, "false") || !strcmp (buf,"no"))
		return false;

	fprintf (stderr, "resource value must be boolean, not \"%s\".\n", buf);
	return false;
}
		
Options::Option::operator int ()const
{
	if(NULL == value)
	{
		fprintf (stderr, "resource not found, return default value: 0.\n");
		return 0;
	}

	const char* s = value;
	while (*s && *s <= ' ') s++;

	int val = 0;
	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))	// 0x: parse as hex
	{
		if (0 < sscanf (s, "%X", &val))
		{
			return val;
		}
	}
	else							// else parse as dec
	{
		if (0 < sscanf (s, "%d", &val))
		{
			return val;
		}
	}

	fprintf (stderr, "resource value must be an integer  [%s]\n", value);
	return 0;
}

Options::Option::operator float ()const
{
	if(NULL == value)
	{
		fprintf (stderr, "resource not found, return default value: 0.0f.\n");
		return 0.0f;
	}

	float val=0.0f;
	if (0 < sscanf (value, " %f", &val))
	{
		return val;
	}
	fprintf (stderr, "resource value must be a float\n");
	return 0.0f;
}

Options::Option::operator double ()const
{
	if(NULL == value)
	{
		fprintf (stderr, "resource not found, return default value: 0.0.\n");
		return 0.0;
	}

	double val;
	if (0 < sscanf (value, " %lf", &val))
	{
		return val;
	}
	fprintf (stderr, "resource value must be a double\n");
	return 0.0;
}

bool Options::load(int argc, char **argv, Option* defaults)
{
	if(NULL == options)
	{
		options = defaults;
		// search arguments and modify default values if it needed
		for(int i=0; options[i].name; i++)
		{
			for(int j=1; j<argc; j++)
			{
				if( char* p=strstr(argv[j], options[i].name) )
				{
					if( char* s=strchr(p,'=') )
					{
						options[i].value = s+1;
					}
					else options[i].value = "true";
				}
			}
		}
		return true;
	}
	return false;
}

const Options::Option& Options::get(const char* name)
{
	int i=0;
	for(; options[i].name; i++)
	{
		if( 0 == strcmp(options[i].name, name) )
		{
			break;
		}
	}

	return options[i];
}
//--------------------------------------------------------------

