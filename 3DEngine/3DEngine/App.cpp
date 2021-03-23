#include "App.h"
#include <assert.h>
#include <memory>
#include <algorithm>
#include <random>
#include "ChiliMath.h"
#include "Surface.h"
#include "imgui/imgui.h"
#include "VertexBuffer.h"
#include "ChiliUtil.h"

namespace dx = DirectX;

ID3D11UnorderedAccessView* backBufferUAV = nullptr;
ID3DBlob* grayscaleComputeBlob = nullptr;
ID3D11ComputeShader* grayscaleComputeShader = nullptr;

// compute particles
ID3D11Buffer* computeParticlesBuffer = nullptr;
ID3D11Buffer* computeParticlesCB = nullptr;
ID3D11ShaderResourceView* computeParticlesSRV = nullptr;
ID3D11UnorderedAccessView* computeParticlesUAV = nullptr;
ID3DBlob* computeParticlesBlob = nullptr;
ID3D11ComputeShader* computeParticlesShader = nullptr;
ID3DBlob* computeParticlesVSBlob = nullptr;
ID3D11VertexShader* computeParticlesVS = nullptr;
ID3DBlob* computeParticlesPSBlob = nullptr;
ID3D11PixelShader* computeParticlesPS = nullptr;
ID3D11Buffer* computeParticlesVSConstants = nullptr;

struct ComputeParticleConstants
{
	float centerOfGravity[3];
	float gravityStrength;
	float deltaTime;
	unsigned numTotalParticles;
	float pad[2];
};

struct ComputeParticle
{
	float position[3];
	float velocity[3];
	float acceleration[3];
	float mass;
};

static const unsigned numComputeParticles = 100000;
static const float particlesGravityStrength = 9.8f;

template<typename _Type>
void SafeRelease( _Type*& r )
{
	if ( r )
	{
		r->Release();
		r = nullptr;
	}
}

bool CreateShader( ID3D11Device* device, ID3DBlob* shaderBlob, ID3D11VertexShader*& shader )
{
	HRESULT hr = device->CreateVertexShader( shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &shader );
	assert( SUCCEEDED( hr ) );
	return SUCCEEDED( hr );
}

bool CreateShader( ID3D11Device* device, ID3DBlob* shaderBlob, ID3D11PixelShader*& shader )
{
	HRESULT hr = device->CreatePixelShader( shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &shader );
	assert( SUCCEEDED( hr ) );
	return SUCCEEDED( hr );
}

bool CreateShader( ID3D11Device* device, ID3DBlob* shaderBlob, ID3D11ComputeShader*& shader )
{
	HRESULT hr = device->CreateComputeShader( shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &shader );
	assert( SUCCEEDED( hr ) );
	return SUCCEEDED( hr );
}

template<typename _ShaderType>
bool LoadShader( ID3D11Device* device, const char* shaderPath, ID3DBlob*& shaderBlob, _ShaderType*& shader )
{
	SafeRelease( shader );
	SafeRelease( shaderBlob );

	HRESULT hr = D3DReadFileToBlob( ToWide( shaderPath ).c_str(), &shaderBlob );
	assert( SUCCEEDED( hr ) );
	if ( FAILED( hr ) )
		return false;

	return CreateShader( device, shaderBlob, shader );
}

float GenRandFloat( std::mt19937& generator, float rangeMin, float rangeMax )
{
	unsigned randVal = generator();
	float normalizedRandVal = ( randVal & 0x7FFFFF ) / 8388607.0f;
	float randFloat = rangeMin + ( ( rangeMax - rangeMin ) * normalizedRandVal );
	assert( randFloat >= rangeMin && randFloat <= rangeMax );
	return randFloat;
}

App::App( const std::string& commandLine )
	:
	commandLine( commandLine ),
	wnd( 1280, 720, "The Donkey Fart Box" ),
	scriptCommander( TokenizeQuoted( commandLine ) ),
	light( wnd.Gfx() )
{
	//wall.SetRootTransform( dx::XMMatrixTranslation( -12.0f,0.0f,0.0f ) );
	//tp.SetPos( { 12.0f,0.0f,0.0f } );
	//gobber.SetRootTransform( dx::XMMatrixTranslation( 0.0f,0.0f,-4.0f ) );
	//nano.SetRootTransform( dx::XMMatrixTranslation( 0.0f,-7.0f,6.0f ) );
	//bluePlane.SetPos( cam.GetPos() );
	//redPlane.SetPos( cam.GetPos() );

	wnd.Gfx().SetProjection( dx::XMMatrixPerspectiveLH( 1.0f, 9.0f / 16.0f, 0.5f, 400.0f ) );

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	HRESULT hr = wnd.Gfx().GetDevice()->CreateUnorderedAccessView( wnd.Gfx().GetTargetTexture(), &uavDesc, &backBufferUAV );
	assert( SUCCEEDED( hr ) );

	LoadShader( wnd.Gfx().GetDevice(), "GrayscaleCompute.cso", grayscaleComputeBlob, grayscaleComputeShader );

	// compute particles stuffs
	LoadShader( wnd.Gfx().GetDevice(), "ComputeParticles.cso", computeParticlesBlob, computeParticlesShader );
	LoadShader( wnd.Gfx().GetDevice(), "ComputeParticlesVS.cso", computeParticlesVSBlob, computeParticlesVS );
	LoadShader( wnd.Gfx().GetDevice(), "ComputeParticlesPS.cso", computeParticlesPSBlob, computeParticlesPS );

	ComputeParticle* computeParticlesData = new ComputeParticle[numComputeParticles];

	std::mt19937 rng;
	float* centerOfGravity = light.GetMesh().GetTransformXM().r[3].m128_f32;
	for ( int i = 0; i < numComputeParticles; i++ )
	{
		ComputeParticle& particle = computeParticlesData[i];
		for ( int j = 0; j < 3; j++ )
		{
			particle.position[j] = centerOfGravity[j] + GenRandFloat( rng, -5, 5 );
			particle.velocity[j] = GenRandFloat( rng, -10, 10 );
			particle.acceleration[j] = 0;
		}
		particle.mass = GenRandFloat( rng, 0.1, 1 );
	}

	D3D11_BUFFER_DESC particlesBufferDesc = {};
	particlesBufferDesc.ByteWidth = sizeof( ComputeParticle ) * numComputeParticles;
	particlesBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	particlesBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	particlesBufferDesc.CPUAccessFlags = 0;
	particlesBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	particlesBufferDesc.StructureByteStride = sizeof( ComputeParticle );

	D3D11_SUBRESOURCE_DATA initialData = {};
	initialData.pSysMem = computeParticlesData;
	hr = wnd.Gfx().GetDevice()->CreateBuffer( &particlesBufferDesc, &initialData, &computeParticlesBuffer );
	assert( SUCCEEDED( hr ) );
	delete[] computeParticlesData;

	D3D11_SHADER_RESOURCE_VIEW_DESC particlesSrvDesc = {};
	particlesSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	particlesSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	particlesSrvDesc.Buffer.FirstElement = 0;
	particlesSrvDesc.Buffer.NumElements = numComputeParticles;
	hr = wnd.Gfx().GetDevice()->CreateShaderResourceView( computeParticlesBuffer, &particlesSrvDesc, &computeParticlesSRV );
	assert( SUCCEEDED( hr ) );

	D3D11_UNORDERED_ACCESS_VIEW_DESC particlesUavDesc = {};
	particlesUavDesc.Format = DXGI_FORMAT_UNKNOWN;
	particlesUavDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_BUFFER;
	particlesUavDesc.Buffer.FirstElement = 0;
	particlesUavDesc.Buffer.NumElements = numComputeParticles;
	particlesUavDesc.Buffer.Flags = 0;
	hr = wnd.Gfx().GetDevice()->CreateUnorderedAccessView( computeParticlesBuffer, &particlesUavDesc, &computeParticlesUAV );
	assert( SUCCEEDED( hr ) );

	D3D11_BUFFER_DESC particlesCBDesc = {};
	particlesCBDesc.ByteWidth = sizeof( ComputeParticleConstants );
	particlesCBDesc.Usage = D3D11_USAGE_DYNAMIC;
	particlesCBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	particlesCBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	particlesCBDesc.MiscFlags = 0;
	particlesCBDesc.StructureByteStride = 0;
	hr = wnd.Gfx().GetDevice()->CreateBuffer( &particlesCBDesc, nullptr, &computeParticlesCB );
	assert( SUCCEEDED( hr ) );

	D3D11_BUFFER_DESC computeParticlesVSConstantsDesc = {};
	computeParticlesVSConstantsDesc.ByteWidth = sizeof( DirectX::XMMATRIX );
	computeParticlesVSConstantsDesc.Usage = D3D11_USAGE_DYNAMIC;
	computeParticlesVSConstantsDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	computeParticlesVSConstantsDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	computeParticlesVSConstantsDesc.MiscFlags = 0;
	computeParticlesVSConstantsDesc.StructureByteStride = 0;
	hr = wnd.Gfx().GetDevice()->CreateBuffer( &computeParticlesVSConstantsDesc, nullptr, &computeParticlesVSConstants );
	assert( SUCCEEDED( hr ) );
}

void App::DoFrame()
{
	const auto dt = timer.Mark() * speed_factor;
	wnd.Gfx().BeginFrame( 0.07f, 0.0f, 0.12f );
	wnd.Gfx().SetCamera( cam.GetMatrix() );
	light.Bind( wnd.Gfx(), cam.GetMatrix() );

	//wall.Draw( wnd.Gfx() );
	//tp.Draw( wnd.Gfx() );
	//nano.Draw( wnd.Gfx() );
	//gobber.Draw( wnd.Gfx() );
	light.Draw( wnd.Gfx() );
	sponza.Draw( wnd.Gfx() );
	//bluePlane.Draw( wnd.Gfx() );
	//redPlane.Draw( wnd.Gfx() );

	while ( const auto e = wnd.kbd.ReadKey() )
	{
		if ( !e->IsPress() )
		{
			continue;
		}
		switch ( e->GetCode() )
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

	if ( wnd.kbd.KeyIsPressed( 'L' ) )
	{
		LoadShader( wnd.Gfx().GetDevice(), "GrayscaleCompute.cso", grayscaleComputeBlob, grayscaleComputeShader );
		LoadShader( wnd.Gfx().GetDevice(), "ComputeParticles.cso", computeParticlesBlob, computeParticlesShader );
		LoadShader( wnd.Gfx().GetDevice(), "ComputeParticlesVS.cso", computeParticlesVSBlob, computeParticlesVS );
		LoadShader( wnd.Gfx().GetDevice(), "ComputeParticlesPS.cso", computeParticlesPSBlob, computeParticlesPS );
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
			cam.Rotate( (float)delta->x, (float)delta->y );
		}
	}

	// imgui windows
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	//bluePlane.SpawnControlWindow( wnd.Gfx(), "Blue Plane" );
	//redPlane.SpawnControlWindow( wnd.Gfx(), "Red Plane" );
	/*gobber.ShowWindow( wnd.Gfx(),"gobber" );
	wall.ShowWindow( wnd.Gfx(),"Wall" );
	tp.SpawnControlWindow( wnd.Gfx() );
	nano.ShowWindow( wnd.Gfx(),"Nano" );*/
	sponza.ShowWindow( wnd.Gfx(), "Sponza" );

	ID3D11DeviceContext* context = wnd.Gfx().GetContext();
	if ( computeParticlesShader && computeParticlesVS && computeParticlesPS )
	{
		// update our constant buffers first
		D3D11_MAPPED_SUBRESOURCE subrsc;
		HRESULT hr = wnd.Gfx().GetContext()->Map( computeParticlesCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &subrsc );
		assert( SUCCEEDED( hr ) );
		if ( SUCCEEDED( hr ) )
		{
			ID3D11DeviceContext* context = wnd.Gfx().GetContext();

			ComputeParticleConstants& constants = *(ComputeParticleConstants*)subrsc.pData;
			memcpy( constants.centerOfGravity, light.GetMesh().GetTransformXM().r[3].m128_f32, sizeof( constants.centerOfGravity ) );
			constants.gravityStrength = particlesGravityStrength;
			constants.deltaTime = dt;
			constants.numTotalParticles = numComputeParticles;
			context->Unmap( computeParticlesCB, 0 );
		}

		// Update and render particles: will need to set shaders and resources
		hr = wnd.Gfx().GetContext()->Map( computeParticlesVSConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &subrsc );
		assert( SUCCEEDED( hr ) );
		if ( SUCCEEDED( hr ) )
		{
			DirectX::XMMATRIX& mvpMatrix = *( DirectX::XMMATRIX* )subrsc.pData;
			DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant( cam.GetMatrix() );
			DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixInverse( &determinant, cam.GetMatrix() );
			mvpMatrix = cam.GetMatrix() * wnd.Gfx().GetProjection();
			context->Unmap( computeParticlesVSConstants, 0 );
		}

		context->CSSetShader( computeParticlesShader, nullptr, 0 );
		context->CSSetUnorderedAccessViews( 0, 1, &computeParticlesUAV, nullptr );
		context->CSSetConstantBuffers( 0, 1, &computeParticlesCB );

		context->Dispatch( ( (numComputeParticles + 1023) >> 10 ), 1, 1 );

		ID3D11UnorderedAccessView* nullUAV = nullptr;
		context->CSSetUnorderedAccessViews( 0, 1, &nullUAV, nullptr );

		D3D11_PRIMITIVE_TOPOLOGY oldTopology;
		context->IAGetPrimitiveTopology( &oldTopology );

		context->VSSetShader( computeParticlesVS,nullptr,0 );
		context->PSSetShader( computeParticlesPS, nullptr, 0 );
		context->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_POINTLIST );
		context->VSSetConstantBuffers( 0, 1, &computeParticlesVSConstants );
		context->VSSetShaderResources( 0, 1, &computeParticlesSRV );

		ID3D11Buffer* nullBuffer = nullptr;
		UINT stride = 0;
		UINT offset = 0;
		context->IASetVertexBuffers( 0, 1, &nullBuffer, &stride, &offset );
		context->Draw( numComputeParticles, 0 );

		ID3D11ShaderResourceView* nullSRV = nullptr;
		context->VSSetShaderResources( 0, 1, &nullSRV );
		context->IASetPrimitiveTopology( oldTopology );
	}

	if ( grayscaleComputeShader )
	{
		wnd.Gfx().GetContext()->OMSetRenderTargets( 0, nullptr, nullptr );

		wnd.Gfx().GetContext()->CSSetShader( grayscaleComputeShader, nullptr, 0 );
		wnd.Gfx().GetContext()->CSSetUnorderedAccessViews( 0, 1, &backBufferUAV, nullptr );

		unsigned windowWidth = 1280;
		unsigned windowHeight = 720;

		unsigned threadsPerGroupX = 32; // 2^5
		unsigned threadsPerGroupY = 32;

		unsigned dimX = ( windowWidth >> 5 ) + ( ( windowWidth & 31 ) != 0 );
		unsigned dimY = ( windowHeight >> 5 ) + ( ( windowHeight & 31 ) != 0 );
		unsigned dimZ = 1;
		wnd.Gfx().GetContext()->Dispatch( dimX, dimY, dimZ );

		ID3D11RenderTargetView* target = wnd.Gfx().GetTarget();
		wnd.Gfx().GetContext()->OMSetRenderTargets( 1, &target, wnd.Gfx().GetDSV() );
	}

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