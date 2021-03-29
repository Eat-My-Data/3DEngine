#include "App.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "imgui/imgui.h"
#include "VertexBuffer.h"
#include "ChiliUtil.h"

namespace dx = DirectX;

//ID3DBlob* grayscaleComputeBlob = nullptr;
//ID3D11UnorderedAccessView* backBufferUAV = nullptr;
//ID3D11ComputeShader* grayscaleComputeShader = nullptr;

App::App( const std::string& commandLine )
	:
	commandLine( commandLine ),
	wnd( 1280, 720, "The Donkey Fart Box" ),
	scriptCommander( TokenizeQuoted( commandLine ) ),
	dirLight( wnd.Gfx() ),
	light( wnd.Gfx(), 5.0f )
{
	wnd.Gfx().SetProjection( dx::XMMatrixPerspectiveLH( 1.0f,9.0f / 16.0f,0.5f,400.0f ) );

	/*D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	HRESULT hr = wnd.Gfx().GetDevice()->CreateUnorderedAccessView( wnd.Gfx().GetTargetTexture(), &uavDesc, &backBufferUAV );
	
		
	grayscaleComputeBlob = nullptr;
	hr = D3DReadFileToBlob( ToWide( "GrayscaleCompute.cso" ).c_str(), &grayscaleComputeBlob );
	assert( SUCCEEDED( hr ) );
	hr = wnd.Gfx().GetDevice()->CreateComputeShader( grayscaleComputeBlob->GetBufferPointer(), grayscaleComputeBlob->GetBufferSize(), nullptr, &grayscaleComputeShader );
	assert( SUCCEEDED( hr ) );*/
}

void App::DoFrame()
{
	const auto dt = timer.Mark() * speed_factor;
	wnd.Gfx().BeginFrame( 0.07f,0.0f,0.12f );
	wnd.Gfx().SetCamera( cam.GetMatrix() );
	//light.Bind( wnd.Gfx(),cam.GetMatrix() );

	sponza.Draw( wnd.Gfx() );
	light.Draw( wnd.Gfx() );
	dirLight.DrawDirLight( wnd.Gfx() );

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

	/*if ( wnd.kbd.KeyIsPressed( 'L' ) )
	{
		if ( grayscaleComputeShader )
		{
			grayscaleComputeShader->Release();
			grayscaleComputeShader = nullptr;
		}
		
		if ( grayscaleComputeBlob )
		{
			grayscaleComputeBlob->Release();
			grayscaleComputeBlob = nullptr;
		}
		HRESULT hr = D3DReadFileToBlob( ToWide( "GrayscaleCompute.cso" ).c_str(), &grayscaleComputeBlob );
		assert( SUCCEEDED( hr ) );
		hr = wnd.Gfx().GetDevice()->CreateComputeShader( grayscaleComputeBlob->GetBufferPointer(), grayscaleComputeBlob->GetBufferSize(), nullptr, &grayscaleComputeShader );
		assert( SUCCEEDED( hr ) );
	}*/


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
	//light.SpawnControlWindow();
	sponza.ShowWindow( wnd.Gfx(),"Sponza" );

	/*if ( grayscaleComputeShader )
	{
		wnd.Gfx().GetContext()->OMSetRenderTargets( 0, nullptr, nullptr );

		wnd.Gfx().GetContext()->CSSetShader( grayscaleComputeShader, nullptr, 0 );
		wnd.Gfx().GetContext()->CSSetUnorderedAccessViews( 0, 1, &backBufferUAV, nullptr );

		unsigned windowHeight = 720;
		unsigned windowWidth = 1280;

		unsigned threadsPerGroupX = 32;
		unsigned threadsPerGroupY = 32;

		unsigned dimX = ( windowWidth >> 5 ) + ( ( windowWidth & 31 ) != 0 );
		unsigned dimY = ( windowHeight >> 5 ) + ( ( windowHeight & 31 ) != 0 );
		unsigned dimZ = 1;
		wnd.Gfx().GetContext()->Dispatch( dimX, dimY, dimZ );

		ID3D11RenderTargetView* target = wnd.Gfx().GetTarget();
		wnd.Gfx().GetContext()->OMSetRenderTargets( 1, &target, wnd.Gfx().GetDSV() );
	}*/
	

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