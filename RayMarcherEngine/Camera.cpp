#include "pch.h"
#include "Camera.h"

Camera::Camera(DirectX::XMFLOAT4 position, DirectX::XMFLOAT4 lookAt, DirectX::XMFLOAT4 up, CAMERA_TYPE cameraType, float aspectRatio, float fov, float nearPlane, float farPlane) :
	m_Position(position), m_LookTo(lookAt), m_Up(up), m_CameraType(cameraType), m_AspectRatio(aspectRatio), m_FOV(fov), m_NearPlane(nearPlane), m_FarPlane(farPlane), m_BackgroundColour(DirectX::Colors::MidnightBlue)
{ }

Camera::~Camera() { }

void Camera::SetCameraType(CAMERA_TYPE val)
{
	m_CameraType = val;
}

void Camera::SetCameraPosition(DirectX::XMFLOAT4 val) {
	m_Position = val;
}

void Camera::SetCameraLookTo(DirectX::XMFLOAT4 val) {
	m_LookTo = val;
}

void Camera::SetCameraUp(DirectX::XMFLOAT4 val) {
	m_Up = val;
}

void Camera::SetFOV(float val) {
	m_FOV = val;
}

void Camera::SetNearPlane(float val) {
	m_NearPlane = val;
}

void Camera::SetFarPlane(float val) {
	m_FarPlane = val;
}

void Camera::SetBackgroundColour(DirectX::XMFLOAT3 val) {
	m_BackgroundColour = val;
}

void Camera::SetAspectRatio(float val) {
	m_AspectRatio = val;
}

void Camera::RenderGUIControls() {
	if (ImGui::CollapsingHeader("Active Camera Controls")) {
		ImGui::DragFloat3("Eye", &m_Position.x, 0.015f);
		
		ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(ImGui::GetColumnIndex(), ImGui::GetWindowWidth() / 3);
			vec3 lt = vec3(m_LookTo.x, m_LookTo.y, m_LookTo.z);
			if (ImGui::gizmo3D("LookTo", lt, ImGui::GetContentRegionAvailWidth(), imguiGizmo::modeDirection)) 
				m_LookTo = DirectX::XMFLOAT4(lt.x, lt.y, lt.z, m_LookTo.z);
			ImGui::NextColumn();
			ImGui::DragFloat3("LookAt", &m_LookTo.x, 0.015f);
			ImGui::Text("LookTo:\n%f\n%f\n%f", lt.x, lt.y, lt.z);
		ImGui::Columns();

		ImGui::DragFloat3("Up", &m_Up.x, 0.005f);

		ImGui::SliderAngle("FOV", &m_FOV, 5.0f, 160.0f);

		const char* items[] = { "Orthographic", "Perspective" };
		int selection = (int)m_CameraType;
		ImGui::Combo("Camera Type", &selection, items, 2);
		m_CameraType = (CAMERA_TYPE)selection;

		ImGui::DragFloat("Near Plane", &m_NearPlane, 0.01f, 0.1f, 100.0f);
		ImGui::DragFloat("Far Plane", &m_FarPlane, 0.1f, 0.2f, 5000.0f);

		ImGui::ColorEdit3("Background Colour", &m_BackgroundColour.x);
	}
}

DirectX::XMMATRIX Camera::CalculateViewMatrix() {
	DirectX::XMVECTOR eye = DirectX::XMLoadFloat4(&m_Position);
	DirectX::XMVECTOR at = DirectX::XMLoadFloat4(&m_LookTo);
	DirectX::XMVECTOR up = DirectX::XMLoadFloat4(&m_Up);

	return DirectX::XMMatrixLookToLH(eye, at, up);
}