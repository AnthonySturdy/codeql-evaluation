#include "pch.h"
#include "RayMarchingManagerComponent.h"

RayMarchingManagerComponent::RayMarchingManagerComponent(const std::vector<GameObject*>& gameObjects)
	: GameObjects(gameObjects)
{
	CreateConstantBuffers();
}

void RayMarchingManagerComponent::CreateConstantBuffers()
{
	const auto device = DX::DeviceResources::Instance()->GetD3DDevice();

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(RenderSettings);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	DX::ThrowIfFailed(device->CreateBuffer(&bd, nullptr, RenderSettingsConstantBuffer.ReleaseAndGetAddressOf()));

	bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(RayMarchScene);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	DX::ThrowIfFailed(device->CreateBuffer(&bd, nullptr, RayMarchSceneConstantBuffer.ReleaseAndGetAddressOf()));
}

void RayMarchingManagerComponent::Render()
{
	const auto context = DX::DeviceResources::Instance()->GetD3DDeviceContext();

	// Update RenderSettings constant buffer
	context->UpdateSubresource(RenderSettingsConstantBuffer.Get(), 0, nullptr, &RenderSettingsData, 0, 0);
	context->PSSetConstantBuffers(0, 1, RenderSettingsConstantBuffer.GetAddressOf());

	// Update R.M. Scene data constant buffer
	const auto rmObjects = GameObject::FindComponents<RayMarchingManagerComponent>(GameObjects);
}

void RayMarchingManagerComponent::RenderGUI()
{
	const auto ViewportSize = DX::DeviceResources::Instance()->GetOutputSize();
	RenderSettingsData.Resolution[0] = ViewportSize.right;
	RenderSettingsData.Resolution[1] = ViewportSize.bottom;

	ImGui::DragInt("Max Steps", reinterpret_cast<int*>(&RenderSettingsData.MaxSteps), 1.0f, 1, 1000);
	ImGui::DragFloat("Max Dist", &RenderSettingsData.MaxDist, .5f, 1.0f, 10000.0f);
	ImGui::DragFloat("Threshold", &RenderSettingsData.IntersectionThreshold, 0.0001f, 0.0001f, 0.3f);
}
