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
Application::Application():window(NULL)
{
	window = new AppWindow(Options::get("--root"));
	window->clear_background(0x00000000 /*black*/);
}

Application::~Application()
{
	delete window;
}

int Application::run()
{
	Timer timer;
	Scene scene(window);

	try
	{
		Capture capture(640, 480, Options::get("--device"));

		Bitmap target;
		target << capture;
		VideoBuffer frames_stack(target, 0, 100);

		scene.screen.set_video(&frames_stack);
		scene.matrix.set_video(&frames_stack);
	
		while ( window->process_events (&scene) )
		{
			capture();
			frames_stack( target, timer.time() );

			scene.draw();
			scene.tick( timer.tick() );
			scene.swap_buffers();
		}
	}
	catch(Capture::runtime_error& error)
	{
		fprintf(stderr, "capture_error: %p\n", error.what() );
		
		while ( window->process_events (&scene) )
		{
			scene.draw();
			scene.tick( timer.tick() );
			scene.swap_buffers();
		}
	}
		
	return 0;
}
//--------------------------------------------------------------

