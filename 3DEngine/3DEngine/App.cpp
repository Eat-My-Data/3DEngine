#include "App.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include "VertexBuffer.h"
#include "NormalMapTwerker.h"
#include <shellapi.h>

namespace dx = DirectX;

GDIPlusManager gdipm;

App::App( const std::string& commandLine )
	:
	wnd( 1280,720,"The Donkey Fart Box"),
	light( wnd.Gfx() )
{
	// makeshift cli for doing some preprocessing bullshit (so many hacks here)
	if( this->commandLine != "" )
	{
		int nArgs;
		const auto pLineW = GetCommandLineW();
		const auto pArgs = CommandLineToArgvW( pLineW,&nArgs );
		if( nArgs >= 4 && std::wstring(pArgs[1]) == L"--ntwerk-rotx180" )
		{
			const std::wstring pathInWide = pArgs[2];
			const std::wstring pathOutWide = pArgs[3];
			NormalMapTwerker::RotateXAxis180(
				std::string( pathInWide.begin(),pathInWide.end() ),
				std::string( pathOutWide.begin(),pathOutWide.end() )
			);
			throw std::runtime_error( "Normal map processed successfully. Just kidding about that whole runtime error thing." );
		}
	}

	wall.SetRootTransform( dx::XMMatrixTranslation( -12.0f,0.0f,0.0f ) );
	tp.SetPos( { 12.0f,0.0f,0.0f } );
	gobber.SetRootTransform( dx::XMMatrixTranslation( 0.0f,0.0f,-4.0f ) );
	nano.SetRootTransform( dx::XMMatrixTranslation( 0.0f,-7.0f,6.0f ) );

	wnd.Gfx().SetProjection( dx::XMMatrixPerspectiveLH( 1.0f,9.0f / 16.0f,0.5f,40.0f ) );
}

void App::DoFrame()
{
	const auto dt = timer.Mark() * speed_factor;
	wnd.Gfx().BeginFrame( 0.07f,0.0f,0.12f );
	wnd.Gfx().SetCamera( cam.GetMatrix() );
	light.Bind( wnd.Gfx(),cam.GetMatrix() );

	wall.Draw( wnd.Gfx() );
	tp.Draw( wnd.Gfx() );
	nano.Draw( wnd.Gfx() );
	gobber.Draw( wnd.Gfx() );
	light.Draw( wnd.Gfx() );

	while ( const auto e = wnd.kbd.ReadKey() )
	{
		if (!e->IsPress())
		{
			continue;
		}
		switch( e->GetCode() )
		{
		case VK_ESCAPE:
			if ( wnd.CursorEnabled() )
			{
				wnd.DisableCursor();
			}	
			else
			{
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
			break;
		}
	}

	if ( !wnd.CursorEnabled() )
	{
		if ( wnd.kbd.KeyIsPressed( 'W' ) )
		{
			cam.Translate( { 0.0f,0.0f,dt } );
		}
		if ( wnd.kbd.KeyIsPressed( 'A' ) )
		{
			cam.Translate( { -dt,0.0f,0.0f } );
		}
		if ( wnd.kbd.KeyIsPressed( 'S' ) )
		{
			cam.Translate( { 0.0f,0.0f,-dt } );
		}
		if ( wnd.kbd.KeyIsPressed( 'D' ) )
		{
			cam.Translate( { dt,0.0f,0.0f } );
		}
		if ( wnd.kbd.KeyIsPressed( 'R' ) )
		{
			cam.Translate( { 0.0f,dt,0.0f } );
		}
		if ( wnd.kbd.KeyIsPressed( 'F' ) )
		{
			cam.Translate( { 0.0f,-dt,0.0f } );
		}
	}

	while ( const auto delta = wnd.mouse.ReadRawDelta() )
	{
		if ( !wnd.CursorEnabled() )
		{
			cam.Rotate( (float)delta->x,(float)delta->y );
		}
	}

	// imgui windows
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	gobber.ShowWindow( wnd.Gfx(),"gobber" );
	wall.ShowWindow( wnd.Gfx(),"Wall" );
	tp.SpawnControlWindow( wnd.Gfx() );
	nano.ShowWindow( wnd.Gfx(),"Nano" );

	// present
	wnd.Gfx().EndFrame();
}

App::~App()
{}

int App::Go()
{
	while ( true )
	{
		// process all messages pending, but to not block for new messages
		if ( const auto ecode = Window::ProcessMessages() )
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		DoFrame();
	}
}