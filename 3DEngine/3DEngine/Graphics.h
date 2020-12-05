#pragma once
#include "ChiliWin.h"
#include "ChiliException.h"
#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include "DxgiInfoManager.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include <random>
#include "ConditionalNoexcept.h"

namespace Bind
{
	class Bindable;
}

class Graphics
{
	friend class Bind::Bindable;									// need to access binable protected functions
public:
	class Exception : public ChiliException
	{
		using ChiliException::ChiliException;
	};
	class HrException : public Exception
	{
	public:
		HrException( int line,const char* file,HRESULT hr,std::vector<std::string> infoMsgs = {} ) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class InfoException : public Exception
	{
	public:
		InfoException( int line,const char* file,std::vector<std::string> infoMsgs = {} ) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};
public:
	Graphics( HWND hWnd );											// initializes pDevice, pSwap, pContext, pTarget, pDSV, and ImGui
	Graphics( const Graphics& ) = delete;
	Graphics& operator=( const Graphics& ) = delete;
	~Graphics() = default;
	void EndFrame();												// render screen
	void BeginFrame( float red,float green,float blue ) noexcept;	// set up frame
	void DrawIndexed( UINT count )  noxnd;							// draw indexed, non-instanced primitives
	void SetProjection( DirectX::FXMMATRIX proj ) noexcept;			// setter for projection
	DirectX::XMMATRIX GetProjection() const noexcept;				// getter for projection
	void SetCamera( DirectX::FXMMATRIX cam ) noexcept;				// setter for camera
	DirectX::XMMATRIX GetCamera() const noexcept;					// getter for camera
	void EnableImgui() noexcept;									
	void DisableImgui() noexcept;									
	bool IsImguiEnabled() const noexcept;							// get imguiEnabled
private:
	DirectX::XMMATRIX projection;									// projection matrix
	DirectX::XMMATRIX camera;										// camera matrix
	bool imguiEnabled = true;										// allows for configuring objects
#ifndef NDEBUG
	DxgiInfoManager infoManager;									// allows for directx error macros
#endif
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;					// pointer to device 
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;					// pointer to swap chain
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;			// pointer to device context
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;			// pointer to render target view
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;			// poitner to depth stencil view
};
