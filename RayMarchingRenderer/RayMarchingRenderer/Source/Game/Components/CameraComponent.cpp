#include "pch.h"
#include "CameraComponent.h"

#include "DDSTextureLoader.h"
#include "Game/Components/TransformComponent.h"

CameraComponent::CameraComponent()
	: FOV(DirectX::XM_PIDIV2 * 1.25f)
{
	Initialise();
}

CameraComponent::CameraComponent(const float fov)
	: FOV(fov)
{
	Initialise();
}

void CameraComponent::Initialise()
{
	const auto device = DX::DeviceResources::Instance()->GetD3DDevice();

	// Create constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CameraConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	DX::ThrowIfFailed(device->CreateBuffer(&bd, nullptr, ConstantBuffer.ReleaseAndGetAddressOf()));

	// Create sampler
	D3D11_SAMPLER_DESC sampDesc{};
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	DX::ThrowIfFailed(device->CreateSamplerState(&sampDesc, LinearSampler.ReleaseAndGetAddressOf()));
}

void CameraComponent::Render()
{
	const auto context = DX::DeviceResources::Instance()->GetD3DDeviceContext();

	static CameraConstantBuffer ccb = {};
	ccb.View = GetViewMatrix();
	ccb.Position = Parent->GetComponent<TransformComponent>()->GetPosition();
	ccb.FOV = FOV;
	context->UpdateSubresource(ConstantBuffer.Get(), 0, nullptr, &ccb, 0, 0);

	context->PSSetConstantBuffers(1, 1, ConstantBuffer.GetAddressOf());
	context->PSSetShaderResources(0, 1, SkyboxSRV.GetAddressOf());
	context->PSSetSamplers(0, 1, LinearSampler.GetAddressOf());
}

void CameraComponent::RenderGUI()
{
	ImGui::SliderFloat("FOV", &FOV, 1.0f * 0.0174533, 160.0f * 0.0174533);

	// Skybox selection
	ImGui::Image(SkyboxSRV.Get(), ImVec2(20, 20)); // Heightmap preview and tooltip
	if (ImGui::IsItemHovered() && SkyboxSRV.Get())
	{
		ImGui::BeginTooltip();
		ImGui::Image(SkyboxSRV.Get(), ImVec2(200, 200));
		ImGui::EndTooltip();
	}
	ImGui::SameLine();
	static char* path = new char[512]{}; // Heightmap file selection
	ImGui::InputTextWithHint("##", "Select .DDS Skybox Texture", path, 512, ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine();
	if (ImGui::Button("..."))
		ImGuiFileDialog::Instance()->OpenDialog("SelectSkybox", "Choose Skybox", ".dds", ".");
	if (ImGuiFileDialog::Instance()->Display("SelectSkybox"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			const std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();

			// path variable contents displayed in GUI text box
			delete[] path;
			path = new char[512]{};
			for (int i = 0; i < filePath.size(); ++i)
				path[i] = filePath[i];

			// Create SRV
			const auto device = DX::DeviceResources::Instance()->GetD3DDevice();
			//DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(device,
			//                                                    std::wstring(filePath.begin(), filePath.end()).c_str(),
			//                                                    nullptr,
			//                                                    SkyboxSRV.ReleaseAndGetAddressOf()));


			DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFileEx(device, std::wstring(filePath.begin(), filePath.end()).c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, false, nullptr, SkyboxSRV.ReleaseAndGetAddressOf()));
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

DirectX::SimpleMath::Matrix CameraComponent::GetViewMatrix() const
{
	// Modified version of: https://www.braynzarsoft.net/viewtutorial/q16390-19-first-person-camera
	// Convert euler rotation to view matrix

	const TransformComponent* transform = Parent->GetComponent<TransformComponent>();

	const DirectX::SimpleMath::Vector3 eye = transform->GetPosition();
	const DirectX::SimpleMath::Vector3 dir = transform->GetRotation() * 0.01745329f;

	// Pitch and Yaw
	const DirectX::SimpleMath::Matrix camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(dir.x, dir.y, 0);
	DirectX::SimpleMath::Vector3 camTarget = DirectX::XMVector3TransformCoord(DirectX::SimpleMath::Vector3::Forward, camRotationMatrix);
	camTarget.Normalize();
	camTarget += eye;

	// Roll
	const DirectX::SimpleMath::Matrix camYRotationMatrix = DirectX::XMMatrixRotationZ(dir.z);
	const DirectX::SimpleMath::Vector3 camUp = DirectX::XMVector3TransformCoord(DirectX::SimpleMath::Vector3::Up, camYRotationMatrix);

	return DirectX::XMMatrixLookAtLH(eye, camTarget, camUp);
}
