#pragma once

#include "StepTimer.h"
#include "Structures.h"

#include "MeshRenderer.h"
#include "Camera.h"
#include "Scene.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game
{
public:
	Game() noexcept;
	~Game() = default;

	Game(Game&&) = default;
	Game& operator=(Game&&) = default;

	Game(Game const&) = delete;
	Game& operator=(Game const&) = delete;

	// Initialization and management
	void Initialise(HWND window, int width, int height);

	// Basic game loop
	void Tick();

	// Messages
	void OnActivated();
	void OnDeactivated();
	void OnSuspending();
	void OnResuming();
	void OnWindowSizeChanged(int width, int height);
	void OnViewportSizeChanged();

	// Properties
	void GetDefaultSize(int& width, int& height) const noexcept;

private:
	void Update(DX::StepTimer const& timer);
	void Render();

	void SetRenderTargetAndClear(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv);
	void Present();

	void CreateDevice();
	void CreateResources();
	void InitialiseImGui(HWND hwnd);
	void CreateConstantBuffers();
	void CreateCameras(int width, int height);
	void CreateScenes();
	void CreateMeshRenderers();

	void OnDeviceLost();

	// Device resources.
	HWND Window;
	int OutputWidth;
	int OutputHeight;

	D3D_FEATURE_LEVEL FeatureLevel;
	Microsoft::WRL::ComPtr<ID3D11Device1> Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1> Context;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> SwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RTTRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> RTTDepthStencilView;
	ImVec2 ViewportSize;

	// Rendering loop timer.
	DX::StepTimer Timer;

	// ImGui
	ImGuiIO* m_ioImGui = nullptr;

	// Scene
	std::unique_ptr<Scene> scene;
	std::shared_ptr<Camera> ActiveCamera;
	std::shared_ptr<MeshRenderer> RaymarchFullscreenMeshRenderer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> RenderSettingsConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> CameraConstantBuffer;
};
