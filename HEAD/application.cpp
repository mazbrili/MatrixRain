//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	application.cpp
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#include <csignal>

#include "application.h"
#include "app_window.h"
#include "options.h"
#include "bitmap.h"
#include "scene.h"
//--------------------------------------------------------------
Application* Application::sig_handler = NULL;

Application::Application():window(NULL),capture(NULL),running(true)
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

	Application::sig_handler = this;


	struct sigaction action;
	action.sa_handler = &handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	sigaction(SIGTERM, &action, NULL);
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

	Scene scene(window, capture);

	while ( running && window->process_events (&scene) )
	{
		if( fps_limit.time() < 1000000/60 )
		{
			usleep(20);
			continue;
		}

		fps_limit.reset();

		if(capture) capture->capture();

		scene.draw();
		scene.tick( timer.tick() );
		scene.swap_buffers();
	}

	return 0;
}
//--------------------------------------------------------------

