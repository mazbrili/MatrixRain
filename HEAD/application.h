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

	class AppWindow* window;
	class Capture*	capture;
};
//--------------------------------------------------------------
#endif//APPLICATION_H
//--------------------------------------------------------------

