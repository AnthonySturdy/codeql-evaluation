#include "pch.h"
#include "RayMarchingManagerComponent.h"

#include "TransformComponent.h"

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
	const auto ViewportSize = DX::DeviceResources::Instance()->GetViewportSize();
	RenderSettingsData.Resolution[0] = ViewportSize.right;
	RenderSettingsData.Resolution[1] = ViewportSize.bottom;
	context->UpdateSubresource(RenderSettingsConstantBuffer.Get(), 0, nullptr, &RenderSettingsData, 0, 0);
	context->PSSetConstantBuffers(0, 1, RenderSettingsConstantBuffer.GetAddressOf());

	// Update R.M. Scene data constant buffer
	const auto rmObjects = GameObject::FindComponents<RayMarchObjectComponent>(GameObjects);
	for (int i = 0; i < RAYMARCH_MAX_OBJECTS; ++i)
	{
		// Populate GPU cbuffer with object data
		if (i < rmObjects.size())
		{
			const auto transform = rmObjects[i]->Parent->GetComponent<TransformComponent>();
			RayMarchSceneData.ObjectsList[i].Position = transform->GetPosition();
			RayMarchSceneData.ObjectsList[i].Rotation = transform->GetRotation();
			RayMarchSceneData.ObjectsList[i].Scale = transform->GetScale();
			RayMarchSceneData.ObjectsList[i].Parameters = rmObjects[i]->GetParameters();
			RayMarchSceneData.ObjectsList[i].SDFType = rmObjects[i]->GetSDFType();
			RayMarchSceneData.ObjectsList[i].BoolOperator = rmObjects[i]->GetBoolOperator();
		}
		else
		{
			// Clear if object data not in use
			RayMarchSceneData.ObjectsList[i] = {};
		}
	}
	context->UpdateSubresource(RayMarchSceneConstantBuffer.Get(), 0, nullptr, &RayMarchSceneData, 0, 0);
	context->PSSetConstantBuffers(2, 1, RayMarchSceneConstantBuffer.GetAddressOf());
}

void RayMarchingManagerComponent::RenderGUI()
{
	ImGui::DragInt("Max Steps", reinterpret_cast<int*>(&RenderSettingsData.MaxSteps), 1.0f, 1, 1000);
	ImGui::DragFloat("Max Dist", &RenderSettingsData.MaxDist, .5f, 1.0f, 10000.0f);
	ImGui::DragFloat("Threshold", &RenderSettingsData.IntersectionThreshold, 0.0001f, 0.0001f, 0.3f);
}
