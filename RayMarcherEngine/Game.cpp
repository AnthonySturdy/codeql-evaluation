#include "pch.h"
#include "Game.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept :
	Window(nullptr),
	OutputWidth(1280),
	OutputHeight(720),
	FeatureLevel(D3D_FEATURE_LEVEL_9_1) {}

// Initialize the Direct3D resources required to run.
void Game::Initialise(HWND window, int width, int height)
{
	Window = window;
	OutputWidth = std::max(width, 1);
	OutputHeight = std::max(height, 1);

	CreateDevice();

	CreateResources();

	InitialiseImGui(window);

	CreateConstantBuffers();

	CreateCameras(width, height);

	CreateMeshRenderers();

	CreateScenes();

	// Enable vsync
	//m_timer.SetFixedTimeStep(true);
	//m_timer.SetTargetElapsedSeconds(1.0 / 60);
}

// Executes the basic game loop.
void Game::Tick()
{
	Timer.Tick([&]() {
		Update(Timer);
	});

	Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	float elapsedTime = static_cast<float>(timer.GetElapsedSeconds());

	scene->Update(elapsedTime, Device.Get());
}

// Draws the scene.
void Game::Render()
{
	// Don't try to render anything before the first Update.
	if (Timer.GetFrameCount() == 0)
	{
		return;
	}

	// Bind render target to intermediate RenderTargetView
	SetRenderTargetAndClear(RTTRenderTargetView.Get(), RTTDepthStencilView.Get());

	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Create ImGui dockspace
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

	scene->Render(Context.Get());

	// Set active vertex layout
	const auto shader = RaymarchFullscreenMeshRenderer->GetShader();
	Context->IASetInputLayout(shader->GetVertexLayout().Get());

	// Update constant buffer
	static ConstantBuffer::RenderSettings rs;
	rs.resolution[0] = ViewportSize.x;
	rs.resolution[1] = ViewportSize.y;
	ImGui::Begin("Render Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::DragInt("Max Steps", reinterpret_cast<int*>(&rs.maxSteps), 1.0f, 1, 1000);
	ImGui::DragFloat("Max Dist", &rs.maxDist, .5f, 1.0f, 10000.0f);
	ImGui::DragFloat("Threshold", &rs.intersectionThreshold, 0.0001f, 0.0001f, 0.3f);
	ImGui::End();

	Context->UpdateSubresource(RenderSettingsConstantBuffer.Get(), 0, nullptr, &rs, 0, 0);


	//static ConstantBuffer cb1;
	//cb1.renderSettings = rs;
	//cb1.camera = cam;
	//ImGui::Begin("Scene Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	//for (int i = 0; i < OBJECT_COUNT; i++)
	//{
	//	if (i != 0 && !cb1.object[i - 1].isActive)
	//		break;
	//
	//	if (ImGui::CollapsingHeader((std::string("Object ") + std::to_string(i)).c_str()))
	//	{
	//		ConstantBuffer::WorldObject obj = cb1.object[i];
	//
	//		ImGui::PushID(i);
	//		ImGui::Checkbox("IsActive", reinterpret_cast<bool*>(&obj.isActive));
	//		ImGui::DragFloat3("Position", &obj.position[0], 0.015f);
	//		const char* items[] = { "Sphere", "Box", "Torus", "Cone", "Cylinder" };
	//		int selection = obj.objectType;
	//		ImGui::Combo("Camera Type", &selection, items, 5);
	//		obj.objectType = selection;
	//		switch (obj.objectType)
	//		{
	//		case 0: // Sphere
	//			ImGui::DragFloat("Radius", &obj.params[0], 0.015f, 0.0f, D3D11_FLOAT32_MAX);
	//			break;
	//		case 1: // Box
	//			ImGui::DragFloat3("Size", &obj.params[0], 0.015f, 0.0f, D3D11_FLOAT32_MAX);
	//			break;
	//		case 2: // Torus
	//			ImGui::DragFloat("Radius", &obj.params[0], 0.015f, 0.0f, D3D11_FLOAT32_MAX);
	//			ImGui::DragFloat("Thickness", &obj.params[1], 0.015f, 0.0f, D3D11_FLOAT32_MAX);
	//			break;
	//		case 3: // Cone
	//			ImGui::DragFloat("Angle Sin", &obj.params[0], 0.015f);
	//			ImGui::DragFloat("Angle Cos", &obj.params[1], 0.015f);
	//			ImGui::DragFloat("Height", &obj.params[2], 0.015f, 0.0f, D3D11_FLOAT32_MAX);
	//			break;
	//		case 4: // Cylinder
	//			ImGui::DragFloat("Radius", &obj.params[0], 0.015f, 0.0f, D3D11_FLOAT32_MAX);
	//			ImGui::DragFloat("Height", &obj.params[1], 0.015f, 0.0f, D3D11_FLOAT32_MAX);
	//			break;
	//		default:
	//			ImGui::DragFloat3("Params", &obj.params[0], 0.015f);
	//			break;
	//		}
	//		ImGui::PopID();
	//
	//		cb1.object[i] = obj;
	//	}
	//}
	//ImGui::End();

	static ConstantBuffer::WorldCamera cam;
	cam.position[0] = ActiveCamera->GetCameraPosition().x;
	cam.position[1] = ActiveCamera->GetCameraPosition().y;
	cam.position[2] = ActiveCamera->GetCameraPosition().z;
	cam.fov = ActiveCamera->GetFOV();
	cam.cameraType = static_cast<int>(ActiveCamera->GetCameraType());
	cam.view = ActiveCamera->CalculateViewMatrix();

	Context->UpdateSubresource(CameraConstantBuffer.Get(), 0, nullptr, &cam, 0, 0);

	// Render the quad
	Context->VSSetShader(shader->GetVertexShader().Get(), nullptr, 0);
	Context->PSSetShader(shader->GetPixelShader().Get(), nullptr, 0);
	Context->PSSetConstantBuffers(0, 1, RenderSettingsConstantBuffer.GetAddressOf());
	Context->PSSetConstantBuffers(1, 1, CameraConstantBuffer.GetAddressOf());

	RaymarchFullscreenMeshRenderer->Render(Context.Get());

	// Bind render target to back buffer
	SetRenderTargetAndClear(RenderTargetView.Get(), DepthStencilView.Get());

	// Create content in ImGui window
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
	ImGui::Begin("Viewport");
	// Detect viewport window resize
	ImVec2 curVpSize = ImGui::GetContentRegionAvail();
	if (ViewportSize.x != curVpSize.x || ViewportSize.y != curVpSize.y)
	{
		ViewportSize = curVpSize;
		OnViewportSizeChanged();
	}

	// Create SRV and render to ImGui window
	ComPtr<ID3D11Resource> resource;
	RTTRenderTargetView->GetResource(resource.ReleaseAndGetAddressOf());
	ComPtr<ID3D11ShaderResourceView> srv;
	Device->CreateShaderResourceView(resource.Get(), nullptr, srv.GetAddressOf());
	ImGui::Image(srv.Get(), ViewportSize);
	ImGui::End();
	ImGui::PopStyleVar();

	// Create frames per second window
	ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("%.3fms (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Begin("Camera Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ActiveCamera->RenderGUIControls();
	RaymarchFullscreenMeshRenderer->RenderGUIControls(Device.Get());
	ImGui::End();

	// Render ImGui
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	if (m_ioImGui->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	Present();
}

// Helper method to clear the back buffers.
void Game::SetRenderTargetAndClear(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv)
{
	// Clear the views.
	XMFLOAT3 camBgCol = ActiveCamera->GetBackgroundColour();
	XMFLOAT4 clearColour = XMFLOAT4(camBgCol.x, camBgCol.y, camBgCol.z, 1.0f);
	Context->ClearRenderTargetView(rtv, &clearColour.x);
	Context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	Context->OMSetRenderTargets(1, &rtv, dsv);

	// Set the viewport.
	CD3D11_VIEWPORT viewport(0.0f, 0.0f, (OutputWidth), (OutputHeight));
	Context->RSSetViewports(1, &viewport);
}

// Presents the back buffer contents to the screen.
void Game::Present()
{
	// The first argument instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.
	HRESULT hr = SwapChain->Present(0, 0);

	// If the device was reset we must completely reinitialize the renderer.
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		OnDeviceLost();
	}
	else
	{
		DX::ThrowIfFailed(hr);
	}
}

// Message handlers
void Game::OnActivated()
{
	// TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
	// TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
	// TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
	Timer.ResetElapsedTime();

	// TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int width, int height)
{
	OutputWidth = std::max(width, 1);
	OutputHeight = std::max(height, 1);

	CreateResources();

	// TODO: Game window is being resized.
}

void Game::OnViewportSizeChanged()
{
	if (ActiveCamera)
		ActiveCamera->SetAspectRatio(ViewportSize.x / ViewportSize.y);

	CreateResources();
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
	// TODO: Change to desired default window size (note minimum size is 320x200).
	width = 1280;
	height = 720;
}

// These are the resources that depend on the device.
void Game::CreateDevice()
{
	UINT creationFlags = 0;

#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	static const D3D_FEATURE_LEVEL featureLevels[] =
	{
		// TODO: Modify for supported Direct3D feature levels
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	// Create the DX11 API device object, and get a corresponding context.
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	DX::ThrowIfFailed(D3D11CreateDevice(
		nullptr, // specify nullptr to use the default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		featureLevels,
		static_cast<UINT>(std::size(featureLevels)),
		D3D11_SDK_VERSION,
		device.ReleaseAndGetAddressOf(), // returns the Direct3D device created
		&FeatureLevel, // returns feature level of device created
		context.ReleaseAndGetAddressOf() // returns the device immediate context
	));

#ifndef NDEBUG
	ComPtr<ID3D11Debug> d3dDebug;
	if (SUCCEEDED(device.As(&d3dDebug)))
	{
		ComPtr<ID3D11InfoQueue> d3dInfoQueue;
		if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
		{
#ifdef _DEBUG
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
			D3D11_MESSAGE_ID hide[] =
			{
				D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
				// TODO: Add more message IDs here as needed.
			};
			D3D11_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
			filter.DenyList.pIDList = hide;
			d3dInfoQueue->AddStorageFilterEntries(&filter);
		}
	}
#endif

	DX::ThrowIfFailed(device.As(&Device));
	DX::ThrowIfFailed(context.As(&Context));

	// TODO: Initialize device dependent objects here (independent of window size).
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateResources()
{
	// Clear the previous window size specific context.
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	Context->OMSetRenderTargets(static_cast<UINT>(std::size(nullViews)), nullViews, nullptr);
	RenderTargetView.Reset();
	DepthStencilView.Reset();
	Context->Flush();

	const UINT backBufferWidth = static_cast<UINT>(OutputWidth);
	const UINT backBufferHeight = static_cast<UINT>(OutputHeight);
	const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
	const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	constexpr UINT backBufferCount = 2;

	// If the swap chain already exists, resize it, otherwise create one.
	if (SwapChain)
	{
		HRESULT hr = SwapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			// If the device was removed for any reason, a new device and swap chain will need to be created.
			OnDeviceLost();

			// Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
			// and correctly set up the new device.
			return;
		}
		DX::ThrowIfFailed(hr);
	}
	else
	{
		// First, retrieve the underlying DXGI Device from the D3D Device.
		ComPtr<IDXGIDevice1> dxgiDevice;
		DX::ThrowIfFailed(Device.As(&dxgiDevice));

		// Identify the physical adapter (GPU or card) this device is running on.
		ComPtr<IDXGIAdapter> dxgiAdapter;
		DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

		// And obtain the factory object that created it.
		ComPtr<IDXGIFactory2> dxgiFactory;
		DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

		// Create a descriptor for the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = backBufferWidth;
		swapChainDesc.Height = backBufferHeight;
		swapChainDesc.Format = backBufferFormat;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = backBufferCount;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
		fsSwapChainDesc.Windowed = TRUE;

		// Create a SwapChain from a Win32 window.
		DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
			Device.Get(),
			Window,
			&swapChainDesc,
			&fsSwapChainDesc,
			nullptr,
			SwapChain.ReleaseAndGetAddressOf()
		));

		// This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
		DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(Window, DXGI_MWA_NO_ALT_ENTER));
	}

	// Obtain the backbuffer for this window which will be the final 3D rendertarget.
	ComPtr<ID3D11Texture2D> m_backBuffer;
	DX::ThrowIfFailed(SwapChain->GetBuffer(0, IID_PPV_ARGS(m_backBuffer.GetAddressOf())));

	// Create a view interface on the rendertarget to use on bind.
	DX::ThrowIfFailed(Device->CreateRenderTargetView(m_backBuffer.Get(), nullptr, RenderTargetView.ReleaseAndGetAddressOf()));

	// Allocate a 2-D surface as the depth/stencil buffer and
	// create a DepthStencil view on this surface to use on bind.
	CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

	ComPtr<ID3D11Texture2D> depthStencil;
	DX::ThrowIfFailed(Device->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	DX::ThrowIfFailed(Device->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, DepthStencilView.ReleaseAndGetAddressOf()));

	// Create intermediate render texture and depth stencil
	Microsoft::WRL::ComPtr<ID3D11Texture2D> rttTex;
	D3D11_TEXTURE2D_DESC rttDesc = {};
	rttDesc.Width = backBufferWidth;
	rttDesc.Height = backBufferHeight;
	rttDesc.MipLevels = 1;
	rttDesc.ArraySize = 1;
	rttDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rttDesc.SampleDesc.Count = 1;
	rttDesc.Usage = D3D11_USAGE_DEFAULT;
	rttDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	rttDesc.CPUAccessFlags = 0;
	rttDesc.MiscFlags = 0;
	DX::ThrowIfFailed(Device->CreateTexture2D(&rttDesc, nullptr, rttTex.GetAddressOf()));
	DX::ThrowIfFailed(Device->CreateRenderTargetView(rttTex.Get(), nullptr, RTTRenderTargetView.ReleaseAndGetAddressOf()));

	ComPtr<ID3D11Texture2D> rttDepthStencil;
	DX::ThrowIfFailed(Device->CreateTexture2D(&depthStencilDesc, nullptr, rttDepthStencil.GetAddressOf()));
	DX::ThrowIfFailed(Device->CreateDepthStencilView(rttDepthStencil.Get(), &depthStencilViewDesc, RTTDepthStencilView.ReleaseAndGetAddressOf()));

	// TODO: Initialize windows-size dependent objects here.
}

void Game::InitialiseImGui(HWND hwnd)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	m_ioImGui = &ImGui::GetIO();
	(void)m_ioImGui;
	m_ioImGui->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	m_ioImGui->ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
	m_ioImGui->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

	ImGui::StyleColorsDark();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (m_ioImGui->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(Device.Get(), Context.Get());
}

void Game::CreateConstantBuffers()
{
	// Create the constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer::RenderSettings);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	Device->CreateBuffer(&bd, nullptr, RenderSettingsConstantBuffer.GetAddressOf());

	bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer::WorldCamera);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	Device->CreateBuffer(&bd, nullptr, CameraConstantBuffer.GetAddressOf());
}

void Game::CreateCameras(int width, int height)
{
	// Create camera
	ActiveCamera = std::make_shared<Camera>(XMFLOAT4(0.0f, 1.0f, -10.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f),
	                                        ECameraType::Perspective,
	                                        width / static_cast<float>(height),
	                                        XMConvertToRadians(110.0f),
	                                        0.01f, 100.0f);
}

void Game::CreateScenes()
{
	scene = std::make_unique<Scene>(Device.Get());
	scene->SetShader(RaymarchFullscreenMeshRenderer->GetShader());
}

void Game::CreateMeshRenderers()
{
	// Create and initialise GameObject
	RaymarchFullscreenMeshRenderer = std::make_shared<MeshRenderer>();
	RaymarchFullscreenMeshRenderer->InitMesh(Device.Get(), Context.Get());
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);
	RaymarchFullscreenMeshRenderer->InitShader(Device.Get(), L"VertexShader", L"PixelShader", layout, numElements);
}

void Game::OnDeviceLost()
{
	// TODO: Add Direct3D resource cleanup here.

	DepthStencilView.Reset();
	RenderTargetView.Reset();
	SwapChain.Reset();
	Context.Reset();
	Device.Reset();

	CreateDevice();

	CreateResources();
}
