#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include "d3dcompiler.h"
#include <DirectXMath.h>
#include "GraphicsThrowMacros.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui//imgui_impl_win32.h"

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")

Graphics::Graphics( HWND hWnd,int width,int height )
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// for checking results of d3d functions
	HRESULT hr;

	// create device and front/back buffers, and swap chain and rendering context
	GFX_THROW_INFO( D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	) );



	//=========================SHADER RESOURCES=========================
	// Initialize the render target texture description.
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target textures.
	for ( int i = 0; i < bufferCount; i++ )
	{
		HRESULT result = pDevice->CreateTexture2D( &textureDesc, NULL, &pTextures[i] );
		if ( FAILED( result ) )
		{
			throw HrException(__LINE__,__FILE__,result);
		}
	}

	// Setup the description of the render target view.
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// back buffer that gets presented
	pSwap->GetBuffer( 0, __uuidof( ID3D11Resource ),&pBackBuffer );
	pDevice->CreateRenderTargetView( pBackBuffer.Get(), nullptr, &lightBuffer );

	// Create the render target views.
	for ( int i = 0; i < bufferCount; i++ )
	{
		HRESULT result = pDevice->CreateRenderTargetView( pTextures[i], &renderTargetViewDesc, &pTarget[i] );
		if ( FAILED( result ) )
		{
			throw HrException( __LINE__, __FILE__, result );
		}
	}

	// Setup the description of the shader resource view.
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource views.
	for ( int i = 0; i < bufferCount; i++ )
	{
		HRESULT result = pDevice->CreateShaderResourceView( pTextures[i], &shaderResourceViewDesc, &pShaderView[i] );
		if ( FAILED( result ) )
		{
			throw HrException( __LINE__, __FILE__, result );
		}
	}
	//=========================SHADER RESOURCES=========================



	//=========================BLEND STATE=========================
	// Setup blend state 
	D3D11_BLEND_DESC blendDescDR;
	ZeroMemory( &blendDescDR, sizeof( blendDescDR ) );
	blendDescDR.AlphaToCoverageEnable = false;
	blendDescDR.IndependentBlendEnable = false;
	blendDescDR.RenderTarget[0].BlendEnable = true;
	blendDescDR.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDescDR.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDescDR.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDescDR.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDescDR.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDescDR.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDescDR.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	pDevice->CreateBlendState( &blendDescDR, &blendState );
	//=========================BLEND STATE=========================



	//=========================DEPTH STENCIL STATES=========================
	// create depth stensil descriptor
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	GFX_THROW_INFO( pDevice->CreateDepthStencilState( &dsDesc,&pDSStateGeometry ) );

	// create depth stensil descriptor
	D3D11_DEPTH_STENCIL_DESC dsDescLight = {};
	dsDesc.DepthEnable = FALSE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_NEVER;
	GFX_THROW_INFO( pDevice->CreateDepthStencilState( &dsDescLight, &pDSStateLighting ) );
	//=========================DEPTH STENCIL STATES=========================



	//=========================DEPTH STENCIL VIEWS=========================
	// create depth stensil texture
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	GFX_THROW_INFO( pDevice->CreateTexture2D( &descDepth,nullptr,&pDepthStencil ) );

	GFX_THROW_INFO( pDevice->CreateTexture2D( &descDepth, nullptr, &pShadowMap ) ); // for shadow map

	// create view of depth stenstil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	GFX_THROW_INFO( pDevice->CreateDepthStencilView(
		pDepthStencil.Get(),&descDSV,&pDSV
	) );

	GFX_THROW_INFO( pDevice->CreateDepthStencilView(
		pShadowMap, &descDSV, &pDSV_ShadowPass
	) );

	descDSV.Flags = D3D11_DSV_READ_ONLY_DEPTH;
	GFX_THROW_INFO( pDevice->CreateDepthStencilView(
		pDepthStencil.Get(), &descDSV, &pDSV_ReadOnlyDepth
	) );
	//=========================DEPTH STENCIL VIEWS=========================



	//=========================DEPTH SHADER RESOURCE=========================
	// create depth shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC depthShaderResourceDesc = {};
	depthShaderResourceDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	depthShaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	depthShaderResourceDesc.Texture2D.MostDetailedMip = 0;
	depthShaderResourceDesc.Texture2D.MipLevels = 1;
	hr = pDevice->CreateShaderResourceView( pDepthStencil.Get(), &depthShaderResourceDesc, &depthShaderView );

	if ( FAILED(hr) )
	{
		throw HrException( __LINE__, __FILE__, hr );
	}
	//=========================DEPTH SHADER RESOURCE=========================



	//=========================SHADOW MAP TEXTURE=========================
	hr = pDevice->CreateTexture2D( &textureDesc, NULL, &pShadowMap );
	if ( FAILED( hr ) )
	{
		throw HrException( __LINE__, __FILE__, hr );
	}

	hr = pDevice->CreateRenderTargetView( pShadowMap, &renderTargetViewDesc, &pShadowMapDepthView );
	if ( FAILED( hr ) )
	{
		throw HrException( __LINE__, __FILE__, hr );
	}

	hr = pDevice->CreateShaderResourceView( pShadowMap, &shaderResourceViewDesc, &pShadowMapSRView );
	if ( FAILED( hr ) )
	{
		throw HrException( __LINE__, __FILE__, hr );
	}
	//=========================SHADOW MAP TEXTURE=========================



	//=========================VIEWPORT=========================
	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pContext->RSSetViewports( 1u,&vp );

	// init imgui d3d impl
	ImGui_ImplDX11_Init( pDevice,pContext );
}

void Graphics::EndFrame()
{
	// imgui frame end
	if ( imguiEnabled )
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );
	}

	HRESULT hr;
	if ( FAILED( hr = pSwap->Present( 1u,0u ) ) )
	{
		if ( hr == DXGI_ERROR_DEVICE_REMOVED )
		{
			throw GFX_DEVICE_REMOVED_EXCEPT( pDevice->GetDeviceRemovedReason() );
		}
		else
		{
			throw GFX_EXCEPT( hr );
		}
	}
}

void Graphics::BeginFrame( float red,float green,float blue ) noexcept
{
	// imgui begin frame
	if ( imguiEnabled )
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	const float color[] = { red,green,blue,1.0f };
	// Clear the render target buffers.
	for ( int i = 0; i < bufferCount; i++ )
	{
		pContext->ClearRenderTargetView( pTarget[i], color );
	} 
	float val[4] = { 0, 0, 0, 0 };
	pContext->ClearRenderTargetView( lightBuffer, val );
	pContext->ClearRenderTargetView( pShadowMapDepthView, val );

	pContext->ClearDepthStencilView( pDSV,D3D11_CLEAR_DEPTH,1.0f,0u );
	pContext->ClearDepthStencilView( pDSV_ShadowPass, D3D11_CLEAR_DEPTH, 1.0f, 0u );
}

void Graphics::DrawIndexed( UINT count ) noxnd
{
	GFX_THROW_INFO_ONLY( pContext->DrawIndexed( count,0u,0u ) );
}

void Graphics::SetProjection( DirectX::FXMMATRIX proj ) noexcept
{
	projection = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return projection;
}

void Graphics::SetCamera( DirectX::FXMMATRIX cam ) noexcept
{
	camera = cam;
}

DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return camera;
}

void Graphics::EnableImgui() noexcept
{
	imguiEnabled = true;
}

void Graphics::DisableImgui() noexcept
{
	imguiEnabled = false;
}

bool Graphics::IsImguiEnabled() const noexcept
{
	return imguiEnabled;
}

void Graphics::BindModelResources() const noexcept
{
	// bind depth state
	pContext->OMSetDepthStencilState( pDSStateGeometry, 1u );

	pContext->OMSetRenderTargets( 3, pTarget, pDSV );
}

void Graphics::BindModelShadowRTV() const noexcept
{
	pContext->OMSetDepthStencilState( pDSStateGeometry, 1u );

	// add ability to write depth values
	pContext->OMSetRenderTargets( 0,nullptr, pDSV_ShadowPass );
}


//================================= EXCEPTIONS =================================//
Graphics::HrException::HrException( int line,const char* file,HRESULT hr,std::vector<std::string> infoMsgs ) noexcept
	:
	Exception( line,file ),
	hr( hr )
{
	// join all info messages with newlines into single string
	for ( const auto& m : infoMsgs )
	{
		info += m;
		info.push_back( '\n' );
	}
	// remove final newline if exists
	if ( !info.empty() )
	{
		info.pop_back();
	}
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if ( !info.empty() )
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "Chili Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString( hr );
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription( hr,buf,sizeof( buf ) );
	return buf;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}

Graphics::InfoException::InfoException( int line,const char* file,std::vector<std::string> infoMsgs ) noexcept
	:
	Exception( line,file )
{
	// join all info messages with newlines into single string
	for ( const auto& m : infoMsgs )
	{
		info += m;
		info.push_back( '\n' );
	}
	// remove final newline if exists
	if ( !info.empty() )
	{
		info.pop_back();
	}
}

const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "Chili Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Chili Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
