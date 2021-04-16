#include "App.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "imgui/imgui.h"
#include "VertexBuffer.h"
#include "ChiliUtil.h"

namespace dx = DirectX;

App::App( const std::string& commandLine )
	:
	commandLine( commandLine ),
	wnd( 1280, 720, "The Donkey Fart Box" ),
	scriptCommander( TokenizeQuoted( commandLine ) ),
	dirLight( wnd.Gfx() ),
	light( wnd.Gfx(), 15.0f )
{
	wnd.Gfx().SetProjection( dx::XMMatrixPerspectiveLH( 1.0f,9.0f / 16.0f,0.5f,400.0f ) );
}

void App::DoFrame()
{
	const auto dt = timer.Mark() * speed_factor;
	wnd.Gfx().BeginFrame( 0.07f,0.0f,0.12f );
	wnd.Gfx().SetCamera( cam.GetMatrix() );

	sponza.Draw( wnd.Gfx() );
	dirLight.DrawDirLight( wnd.Gfx(),cam.GetPos() );
	light.DrawPointLight( wnd.Gfx(), cam.GetMatrix(),cam.GetPos() );

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
		if ( wnd.kbd.KeyIsPressed( 'I' ) )
		{
			light.SetPos( DirectX::XMFLOAT3( 1.0, 0.0, 0.0 ) );
		}
		if ( wnd.kbd.KeyIsPressed( 'K' ) )
		{
			light.SetPos( DirectX::XMFLOAT3( -1.0, 0.0, 0.0 ) );
		}
		if ( wnd.kbd.KeyIsPressed( 'J' ) )
		{
			light.SetPos( DirectX::XMFLOAT3( 0.0, 0.0, 1.0 ) );
		}
		if ( wnd.kbd.KeyIsPressed( 'L' ) )
		{
			light.SetPos( DirectX::XMFLOAT3( 0.0, 0.0, -1.0 ) );
		}
		if ( wnd.kbd.KeyIsPressed( 'O' ) )
		{
			light.SetPos( DirectX::XMFLOAT3( 0.0, 1.0, 0.0 ) );
		}
		if ( wnd.kbd.KeyIsPressed( 'P' ) )
		{
			light.SetPos( DirectX::XMFLOAT3( 0.0, -1.0, 0.0 ) );
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
	//light.SpawnControlWindow();
	sponza.ShowWindow( wnd.Gfx(),"Sponza" );

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