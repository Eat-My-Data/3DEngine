#pragma once
#include "Window.h"
#include "FrameTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "TestPlane.h"
#include "Mesh.h"
#include <set>

class App
{
public:
	App();														
	int Go();													// master frame / message loop
	~App();
private:
	void DoFrame();												// master graphics frame
private:
	bool cursorEnabled = true;									// used to manipulate cursor
	int x = 0,y = 0;											// mouse x and y
	ImguiManager imgui;											// external tool imgui
	Window wnd;													// self contained win32 window, handles input from user
	FrameTimer timer;											// world clock
	float speed_factor = 1.0f;									// time manipulation
	Camera cam;													// configurable view matrix
	PointLight light;											// configurable light source
	Model gobber{ wnd.Gfx(),"Models\\gobber\\GoblinX.obj" };
	// Model wall{ wnd.Gfx(),"Models\\brick_wall\\brick_wall.obj" };
	// TestPlane tp{ wnd.Gfx(),1.0 };
	// Model nano{ wnd.Gfx(),"Models\\nano_textured\\nanosuit.obj" };		// external tool assimp for model loading
};
