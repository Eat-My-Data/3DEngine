#include "Projection.h"
#include "imgui/imgui.h"

Projection::Projection( float width, float height, float nearZ, float farZ )
	:
	width( width ),
	height( height ),
	nearZ( nearZ ),
	farZ( farZ )
{}

DirectX::XMMATRIX Projection::GetMatrix() const
{
	return DirectX::XMMATRIX();
}

void Projection::RenderWidgets()
{
	ImGui::SliderFloat( "Width", &width, 0.1f, 400.0f );
	ImGui::SliderFloat( "Height", &height, 0.1f, 400.0f );
	ImGui::SliderFloat( "Near Z", &nearZ, 0.01f, 400.0f );
	ImGui::SliderFloat( "Far Z", &farZ, 0.01f, 400.0f );
}

Perspective::Perspective( float width, float height, float nearZ, float farZ )
	:
	Projection( width, height, nearZ, farZ )
{}

DirectX::XMMATRIX Perspective::GetMatrix() const
{
	return DirectX::XMMatrixPerspectiveLH( width, height, nearZ, farZ );
}

DirectX::XMMATRIX Perspective::GetCameraMatrix(DirectX::XMFLOAT3 pos,float pitch, float yaw) const
{
	using namespace DirectX;

	const XMVECTOR forwardBaseVector = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
	// apply the camera rotations to a base vector
	const auto lookVector = XMVector3Transform( forwardBaseVector,
		XMMatrixRotationRollPitchYaw( pitch, yaw, 0.0f )
	);
	// generate camera transform (applied to all objects to arrange them relative
	// to camera position/orientation in world) from cam position and direction
	// camera "top" always faces towards +Y (cannot do a barrel roll)
	const auto camPosition = XMLoadFloat3( &pos );
	const auto camTarget = camPosition + lookVector;
	return XMMatrixLookAtLH( camPosition, camTarget, XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
}


Orthogonal::Orthogonal( float width, float height, float nearZ, float farZ )
	:
	Projection( width, height, nearZ, farZ )
{}

DirectX::XMMATRIX Orthogonal::GetMatrix() const
{
	return DirectX::XMMatrixOrthographicLH( width, height, nearZ, farZ );
}

DirectX::XMMATRIX Orthogonal::GetCameraMatrix( DirectX::XMFLOAT3 pos, float pitch, float yaw ) const
{
	using namespace DirectX;

	const XMVECTOR forwardBaseVector = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
	// apply the camera rotations to a base vector
	const auto lookVector = XMVector3Transform( forwardBaseVector,
		XMMatrixRotationRollPitchYaw( pitch, yaw, 0.0f )
	);
	// generate camera transform (applied to all objects to arrange them relative
	// to camera position/orientation in world) from cam position and direction
	// camera "top" always faces towards +Y (cannot do a barrel roll)
	const auto camPosition = XMLoadFloat3( &pos );
	const auto camTarget = camPosition + lookVector;
	//return XMMatrixOrthographicLH(width,height,nearZ,farZ);
	return XMMatrixLookAtLH( camPosition, camTarget, XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
}


