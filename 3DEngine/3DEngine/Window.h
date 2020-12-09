#pragma once
#include "ChiliWin.h"
#include "ChiliException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <optional>
#include <memory>

class Window
{
public:
	class Exception : public ChiliException	
	{
		using ChiliException::ChiliException;
	public:
		static std::string TranslateErrorCode( HRESULT hr ) noexcept;
	};
	class HrException : public Exception
	{
	public:
		HrException( int line,const char* file,HRESULT hr ) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};
	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		const char* GetType() const noexcept override;
	};
private:
	class WindowClass																
	{
	public:
		static const char* GetName() noexcept; 										// getter for windowClassName
		static HINSTANCE GetInstance() noexcept;									// getter for hInst
	private:
		WindowClass() noexcept;
		~WindowClass();																// manages window registration
		WindowClass( const WindowClass& ) = delete;									// cleans up window class
		WindowClass& operator=( const WindowClass& ) = delete;
		static constexpr const char* wndClassName = "Direct3D Engine Window";		// window class name
		static WindowClass wndClass;												// window class object
		HINSTANCE hInst;															// handle to hInstance from WinMain
	};
public:
	Window( int width,int height,const char* name );								// creates the window, initializes imgui and creates graphics object
	~Window();
	Window( const Window& ) = delete;
	Window& operator=( const Window& ) = delete;
	void SetTitle( const std::string& title );										// setter window title
	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	static std::optional<int> ProcessMessages() noexcept;							// message pump
	Graphics& Gfx();																// gives access to graphics pointer
private:
	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;
	void HideCursor() noexcept;
	void ShowCursor() noexcept;
	void EnableImGuiMouse() noexcept;
	void DisableImGuiMouse() noexcept;
	static LRESULT CALLBACK HandleMsgSetup( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam ) noexcept;	// handles initial window creation
	static LRESULT WINAPI HandleMsgThunk( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam ) noexcept;	// forwards to HandleMsg
	LRESULT HandleMsg( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam ) noexcept;						// window message processor
public:
	Keyboard kbd;																	// 	keyboard interface
	Mouse mouse;																	// 	mouse interface
private:
	bool cursorEnabled = false;
	int width;																		// 	window width
	int height;																		// 	window height
	HWND hWnd;																		// 	window object
	std::unique_ptr<Graphics> pGfx;													// 	pointer to graphics object
};