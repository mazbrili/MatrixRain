//--------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:	application.h
// copyright:	(C) 2008 by Pavel Karneliuk
// e-mail:	pavel_karneliuk@users.sourceforge.net
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef APPLICATION_H
#define APPLICATION_H
//--------------------------------------------------------------
//--------------------------------------------------------------
class Application
{
public:
	Application();
	~Application();

	int run();

private:

	void signal(int sig){ running = false; }
	static void handler(int sig)
	{
		if(sig_handler) sig_handler->signal(sig);
	}

	static Application* sig_handler;

	class AppWindow* window;
	class Capture*	capture;
	bool running;
};
//--------------------------------------------------------------
#endif//APPLICATION_H
//--------------------------------------------------------------

