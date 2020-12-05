#pragma once
#include "Window.h"
#include "FrameTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Mesh.h"
#include <set>

class App
{
public:
	App();												// handles window and light construction, sets projection matrix
	int Go();											// master frame / message loop
	~App();
private:
	void DoFrame();										// master graphics frame
private:
	ImguiManager imgui;									// external tool imgui
	Window wnd;											// self contained win32 window, handles input from user
	FrameTimer timer;									// world clock
	float speed_factor = 1.0f;							// time manipulation
	Camera cam;											// configurable view matrix
	PointLight light;									// configurable light source
	Model nano{ wnd.Gfx(),"Models\\nanosuit.obj" };		// external tool assimp for model loading
};
