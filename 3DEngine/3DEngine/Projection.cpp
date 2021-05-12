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
	ImGui::SliderFloat( "Width", &width, 0.01f, 4.0f );
	ImGui::SliderFloat( "Height", &height, 0.01f, 4.0f );
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


Orthogonal::Orthogonal( float width, float height, float nearZ, float farZ )
	:
	Projection( width, height, nearZ, farZ )
{}

DirectX::XMMATRIX Orthogonal::GetMatrix() const
{
	DirectX::XMMATRIX orthoMatrix = DirectX::XMMatrixIdentity();
	orthoMatrix.r[0].m128_f32[0] = 2.0f / width;
	orthoMatrix.r[1].m128_f32[1] = 2.0f / height;
	orthoMatrix.r[2].m128_f32[2] = 1.0f / ( farZ - nearZ );
	orthoMatrix.r[3].m128_f32[2] = nearZ / ( nearZ - farZ );
	return orthoMatrix;
}


