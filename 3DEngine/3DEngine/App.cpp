#include "App.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "imgui/imgui.h"
#include "VertexBuffer.h"
#include "Projection.h"
#include "ChiliUtil.h"

namespace dx = DirectX;

App::App( const std::string& commandLine )
	:
	commandLine( commandLine ),
	wnd( 1280, 720, "The Donkey Fart Box" ),
	scriptCommander( TokenizeQuoted( commandLine ) ),
	dirShadowMap( wnd.Gfx() ),
	dirLight( wnd.Gfx() ),
	light( wnd.Gfx(), 15.0f )
{
	Perspective* pPersp = new Perspective( 1.0f, 9.0f / 16.0f, 0.5f, 400.0f );
	Orthogonal* pOrtho = new Orthogonal( 400.0f, 400.0f, 1.0f, 1200.0f );

	cameras.AddCamera( std::make_unique<Camera>( "A", pPersp, dx::XMFLOAT3{ -13.5f,6.0f,3.5f }, 0.0f, PI / 2.0f ) );
	//cameras.AddCamera( std::make_unique<Camera>( "B", pOrtho, dx::XMFLOAT3{ 0.0f,200.8f,100.0f }, PI / 2.0f, -PI ) );

	cameras.SetDirLightCamera( std::make_unique<Camera>( "B", pOrtho, dx::XMFLOAT3{ 0.0f,200.8f,100.0f }, PI / 2.0f, -PI ) );
}

void App::DoFrame()
{
	const auto dt = timer.Mark() * speed_factor;
	wnd.Gfx().BeginFrame( 0.07f,0.0f,0.12f );

	// gbuffer pass
	cameras.GetCamera().BindToGraphics( wnd.Gfx() );
	wnd.Gfx().BindModelResources();
	sponza.Draw( wnd.Gfx(),false );

	// depth from light pass
	cameras.GetDirLightCamera().BindToGraphics( wnd.Gfx() );
	wnd.Gfx().BindModelShadowRTV();
	sponza.Draw( wnd.Gfx(), true );

	// view projection matrix
	//DirectX::XMMATRIX viewProj = DirectX::XMMatrixMultiply();

	// lighting pass
	cameras.GetCamera().BindToGraphics( wnd.Gfx() );
	dirLight.DrawDirLight( wnd.Gfx(), cameras.GetCamera().GetPos(),cameras.GetDirLightCamera().GetProjection()->GetMatrix(),cameras.GetDirLightCamera().GetProjectionMatrix() );
	light.DrawPointLight( wnd.Gfx(), cameras.GetCamera().GetMatrix(), cameras.GetCamera().GetPos() );

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
			cameras.GetCamera().Translate( { 0.0f,0.0f,dt } );
		}
		if ( wnd.kbd.KeyIsPressed( 'A' ) )
		{
			cameras.GetCamera().Translate( { -dt,0.0f,0.0f } );
		}
		if ( wnd.kbd.KeyIsPressed( 'S' ) )
		{
			cameras.GetCamera().Translate( { 0.0f,0.0f,-dt } );
		}
		if ( wnd.kbd.KeyIsPressed( 'D' ) )
		{
			cameras.GetCamera().Translate( { dt,0.0f,0.0f } );
		}
		if ( wnd.kbd.KeyIsPressed( 'R' ) )
		{
			cameras.GetCamera().Translate( { 0.0f,dt,0.0f } );
		}
		if ( wnd.kbd.KeyIsPressed( 'F' ) )
		{
			cameras.GetCamera().Translate( { 0.0f,-dt,0.0f } );
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
			cameras.GetCamera().Rotate( (float)delta->x,(float)delta->y );
		}
	}

	// imgui windows
	cameras.SpawnWindow();
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