#pragma once

enum class ECameraType
{
	Orthographic = 0,
	Perspective
};

class Camera
{
public:
	Camera(DirectX::XMFLOAT4 position, DirectX::XMFLOAT4 lookAt, DirectX::XMFLOAT4 up, ECameraType cameraType, float aspectRatio, float fov, float nearPlane, float farPlane);
	Camera(const Camera&) = default;
	Camera(Camera&&) = default;
	Camera& operator=(const Camera&) = default;
	Camera& operator=(Camera&&) = default;
	~Camera() = default;

	ECameraType GetCameraType() const { return CameraType; }
	DirectX::XMFLOAT4 GetCameraPosition() const { return Position; }
	DirectX::XMFLOAT4 GetCameraLookAt() const { return LookTo; }
	DirectX::XMFLOAT4 GetCameraUp() const { return Up; }
	float GetFOV() const { return Fov; }
	float GetNearPlane() const { return NearPlane; }
	float GetFarPlane() const { return FarPlane; }
	DirectX::XMFLOAT3 GetBackgroundColour() const { return BackgroundColour; }

	DirectX::XMMATRIX CalculateViewMatrix() const;

	void SetCameraType(ECameraType val);
	void SetCameraPosition(DirectX::XMFLOAT4 val);
	void SetCameraLookTo(DirectX::XMFLOAT4 val);
	void SetCameraUp(DirectX::XMFLOAT4 val);
	void SetFOV(float val);
	void SetNearPlane(float val);
	void SetFarPlane(float val);
	void SetBackgroundColour(DirectX::XMFLOAT3 val);
	void SetAspectRatio(float val);

	void RenderGUIControls();

private:
	ECameraType CameraType;

	DirectX::XMFLOAT4 Position;
	DirectX::XMFLOAT4 LookTo;
	DirectX::XMFLOAT4 Up;
	float Fov;
	float AspectRatio;
	float NearPlane;
	float FarPlane;
	DirectX::XMFLOAT3 BackgroundColour;
};
