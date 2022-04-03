#include "pch.h"
#include "Rendering/RenderPassDefault.h"

#include "Game/GameObject.h"
#include "Game/Components/MaterialComponent.h"
#include "Game/Components/RayMarchLightComponent.h"
#include "Game/Components/RayMarchObjectComponent.h"

RenderPassDefault::RenderPassDefault(std::vector<GameObject*>& gameObjects)
	: GameObjects(gameObjects) {}

void RenderPassDefault::Initialise()
{
	const auto device = DX::DeviceResources::Instance()->GetD3DDevice();
	const auto outputSize = DX::DeviceResources::Instance()->GetViewportSize();

	// Create render textures
	RenderTargetViews.clear();
	Microsoft::WRL::ComPtr<ID3D11Texture2D> rtvTex;
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = outputSize.right;
	texDesc.Height = outputSize.bottom;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	// Main RTV
	DX::ThrowIfFailed(device->CreateTexture2D(&texDesc, nullptr, rtvTex.ReleaseAndGetAddressOf()));
	RenderTargetViews.push_back(nullptr);
	DX::ThrowIfFailed(device->CreateRenderTargetView(rtvTex.Get(), nullptr, RenderTargetViews[RenderTargetViews.size() - 1].ReleaseAndGetAddressOf()));
	// Normal/Depth RTV
	DX::ThrowIfFailed(device->CreateTexture2D(&texDesc, nullptr, rtvTex.ReleaseAndGetAddressOf()));
	RenderTargetViews.push_back(nullptr);
	DX::ThrowIfFailed(device->CreateRenderTargetView(rtvTex.Get(), nullptr, RenderTargetViews[RenderTargetViews.size() - 1].ReleaseAndGetAddressOf()));
	// Reflection Colour and (unnormalised) Depth RTV
	DX::ThrowIfFailed(device->CreateTexture2D(&texDesc, nullptr, rtvTex.ReleaseAndGetAddressOf()));
	RenderTargetViews.push_back(nullptr);
	DX::ThrowIfFailed(device->CreateRenderTargetView(rtvTex.Get(), nullptr, RenderTargetViews[RenderTargetViews.size() - 1].ReleaseAndGetAddressOf()));
	// Metalicness and Roughness RTV
	texDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	DX::ThrowIfFailed(device->CreateTexture2D(&texDesc, nullptr, rtvTex.ReleaseAndGetAddressOf()));
	RenderTargetViews.push_back(nullptr);
	DX::ThrowIfFailed(device->CreateRenderTargetView(rtvTex.Get(), nullptr, RenderTargetViews[RenderTargetViews.size() - 1].ReleaseAndGetAddressOf()));


	// Create depth stencil
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilTex;
	D3D11_TEXTURE2D_DESC depthTexDesc = {};
	depthTexDesc.Width = outputSize.right;
	depthTexDesc.Height = outputSize.bottom;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;
	DX::ThrowIfFailed(device->CreateTexture2D(&depthTexDesc, nullptr, depthStencilTex.GetAddressOf()));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Flags = 0;
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	DX::ThrowIfFailed(device->CreateDepthStencilView(depthStencilTex.Get(), &depthStencilViewDesc, DepthStencilView.ReleaseAndGetAddressOf()));

	// Create render state
	D3D11_RASTERIZER_DESC renderStateDesc = {};
	renderStateDesc.CullMode = D3D11_CULL_BACK;
	renderStateDesc.FillMode = D3D11_FILL_SOLID;
	renderStateDesc.DepthClipEnable = true;
	DX::ThrowIfFailed(device->CreateRasterizerState(&renderStateDesc, RenderState.ReleaseAndGetAddressOf()));

	// Create SRV from RTV
	RenderTargetSRV.clear();
	for (int i = 0; i < RenderTargetViews.size(); ++i)
	{
		RenderTargetSRV.push_back(nullptr);
		Microsoft::WRL::ComPtr<ID3D11Resource> geometryPassResource;
		RenderTargetViews[i]->GetResource(geometryPassResource.ReleaseAndGetAddressOf());
		DX::ThrowIfFailed(device->CreateShaderResourceView(geometryPassResource.Get(), nullptr, RenderTargetSRV[i].ReleaseAndGetAddressOf()));
	}
}

void RenderPassDefault::Render()
{
	const auto context = DX::DeviceResources::Instance()->GetD3DDeviceContext();
	const auto outputSize = DX::DeviceResources::Instance()->GetViewportSize();

	static constexpr DirectX::SimpleMath::Color clearColour(1.0f, 0.0f, 0.0f, 1.0f);
	context->ClearRenderTargetView(RenderTargetViews[0].Get(), &clearColour.x);
	context->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Bind resources
	context->OMSetRenderTargets(RenderTargetViews.size(), RenderTargetViews.data()->GetAddressOf(), DepthStencilView.Get());

	const CD3D11_VIEWPORT viewport(0.0f, 0.0f,
	                               outputSize.right, outputSize.bottom,
	                               0.0f, 1.0f);
	context->RSSetViewports(1, &viewport);

	context->RSSetState(RenderState.Get());

	// Render GameObjects
	for (const auto go : GameObjects)
		go->Render();

	// Unbind render targets
	const std::vector<ID3D11RenderTargetView*> nullRtvs(RenderTargetViews.size(), nullptr);
	ID3D11DepthStencilView* nullDsv = nullptr;
	context->OMSetRenderTargets(RenderTargetViews.size(), nullRtvs.data(), nullDsv);
}

void RenderPassDefault::RenderGUI()
{
	ImGui::Begin("Scene View");

	for (int i = 0; i < GameObjects.size(); ++i)
	{
		const auto go = GameObjects[i];

		ImGui::PushID(go);

		// Remove object
		if (ImGui::Button("X"))
		{
			GameObjects.erase(GameObjects.begin() + i);
			ImGui::PopID();
			break;
		}

		ImGui::SameLine();
		go->RenderGUI();

		ImGui::PopID();
	}

	if (ImGui::Button("Add Ray Marched Object"))
	{
		const auto go = new GameObject("New Ray Marched Object");
		go->AddComponent(new RayMarchObjectComponent());
		go->AddComponent(new MaterialComponent());

		GameObjects.push_back(go);
	}

	if (ImGui::Button("Add Ray Marched Light"))
	{
		const auto go = new GameObject("New Ray Marched Light");
		go->AddComponent(new RayMarchLightComponent());

		GameObjects.push_back(go);
	}

	ImGui::End();
}
