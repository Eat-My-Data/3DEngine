#include "App.h"

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR	  lpCmdLine,
	int		  nCmdShow )
{
	try
	{
		return App{ lpCmdLine }.Go();				// start application
	}
	catch ( const ChiliException& e )	// catch custom exceptions
	{
		MessageBox( nullptr,e.what(),e.GetType(),MB_OK | MB_ICONEXCLAMATION ); 
	}
	catch ( std::exception& e )			// catch standard exceptions
	{
		MessageBox( nullptr,e.what(),"Standard Exception",MB_OK | MB_ICONEXCLAMATION ); 
	}
	catch ( ... )						// catch everything else
	{
		MessageBox( nullptr,"No details available","Unknown Exception",MB_OK | MB_ICONEXCLAMATION ); 
	}
	return -1;
}
