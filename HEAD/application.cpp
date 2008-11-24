//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	application.cpp
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#include "application.h"
#include "app_window.h"
#include "options.h"
#include "bitmap.h"
#include "scene.h"
//--------------------------------------------------------------
Application::Application():window(NULL),capture(NULL)
{
	window = new AppWindow();
	window->clear_background(0x00000000); // black

	try
	{
		capture = new Capture(640, 480, Options::get("--device"));
	}
	catch(Capture::runtime_error& error)
	{
		fprintf(stderr, "capture_error: %s\n", error.what() );
	}
}

Application::~Application()
{
	delete capture;
	delete window;
}

int Application::run()
{
	Timer timer;
	Timer fps_limit;

	Scene scene(window, capture != NULL);

	if(capture != NULL)
	{
		Bitmap target;
		target << *capture;
		VideoBuffer frames_stack(target, 0, 10000);

		scene.matrix->set_video(&frames_stack, capture->width(), capture->height());

		while ( window->process_events (&scene) )
		{
			if( fps_limit.time() < 1000000/60 )
			{
				usleep(20);
			}
			else
			{
				fps_limit.reset();
				(*capture)();
				frames_stack( target, timer.time() );

				scene.draw();
				scene.tick( timer.tick() );
				scene.swap_buffers();
			}
		}
	}
	else
	{
		while ( window->process_events (&scene) )
		{
			if( fps_limit.time() < 1000000/60 )
			{
				usleep(20);
			}
			else
			{
				fps_limit.reset();
	
				scene.draw();
				scene.tick( timer.tick() );
				scene.swap_buffers();
			}
		}
	}
		
	return 0;
}
//--------------------------------------------------------------

