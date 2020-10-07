#include "Window.h"

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR	  lpCmdLine,
	int		  nCmdShow )
{
	try
	{
		Window wnd( 800,300,"Just a name" );

		MSG msg;
		BOOL gResult;
		while ( ( gResult = GetMessage( &msg,nullptr,0,0 ) ) > 0 )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		if ( gResult == -1 )
		{
			return -1;
		}

		return msg.wParam;
	}
	catch ( const SleepyException& e )
	{
		MessageBox( nullptr,e.what(),e.GetType(),MB_OK | MB_ICONEXCLAMATION );
	}
	catch ( std::exception& e )
	{
		MessageBox( nullptr,e.what(),"Standard Exception",MB_OK | MB_ICONEXCLAMATION );
	}
	catch ( ... )
	{
		MessageBox( nullptr,"No details available","Unknown Exception",MB_OK | MB_ICONEXCLAMATION );
	}
	return -1;
}
