#include "pch.h"
#include "Camera.h"

Camera::Camera(DirectX::XMFLOAT4 position, DirectX::XMFLOAT4 lookAt, DirectX::XMFLOAT4 up, ECameraType cameraType, float aspectRatio, float fov, float nearPlane, float farPlane) :
	Position(position), LookTo(lookAt), Up(up), CameraType(cameraType), AspectRatio(aspectRatio), Fov(fov), NearPlane(nearPlane), FarPlane(farPlane), BackgroundColour(DirectX::Colors::MidnightBlue) { }

void Camera::SetCameraType(ECameraType val)
{
	CameraType = val;
}

void Camera::SetCameraPosition(DirectX::XMFLOAT4 val)
{
	Position = val;
}

void Camera::SetCameraLookTo(DirectX::XMFLOAT4 val)
{
	LookTo = val;
}

void Camera::SetCameraUp(DirectX::XMFLOAT4 val)
{
	Up = val;
}

void Camera::SetFOV(float val)
{
	Fov = val;
}

void Camera::SetNearPlane(float val)
{
	NearPlane = val;
}

void Camera::SetFarPlane(float val)
{
	FarPlane = val;
}

void Camera::SetBackgroundColour(DirectX::XMFLOAT3 val)
{
	BackgroundColour = val;
}

void Camera::SetAspectRatio(float val)
{
	AspectRatio = val;
}

void Camera::RenderGUIControls()
{
	ImGui::DragFloat3("Eye", &Position.x, 0.015f);
	ImGui::DragFloat3("LookAt", &LookTo.x, 0.015f);
	ImGui::DragFloat3("Up", &Up.x, 0.005f);

	ImGui::SliderAngle("FOV", &Fov, 91.0f, 180.0f);
}

DirectX::XMMATRIX Camera::CalculateViewMatrix() const
{
	DirectX::XMVECTOR eye = DirectX::XMLoadFloat4(&Position);
	DirectX::XMVECTOR at = DirectX::XMLoadFloat4(&LookTo);
	DirectX::XMVECTOR up = DirectX::XMLoadFloat4(&Up);

	return DirectX::XMMatrixLookAtLH(eye, at, up);
}
