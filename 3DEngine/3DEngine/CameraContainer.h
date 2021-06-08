#pragma once
#include <vector>
#include <memory>

class Camera;
class Graphics;

class CameraContainer
{
public:
	void SpawnWindow();
	void Bind( Graphics& gfx );
	void AddCamera( std::unique_ptr<Camera> pCam );
	void SetDirLightCamera( std::unique_ptr<Camera> pCam );
	Camera& GetCamera();
	Camera& GetDirLightCamera();
	~CameraContainer();
private:
	std::vector<std::unique_ptr<Camera>> cameras;
	Camera* dirLightOrthoCamera;
	int selected = 0;
}; 