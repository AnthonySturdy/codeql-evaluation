#include "pch.h"
#include "RenderPassReflections.h"

RenderPassReflections::RenderPassReflections(const RenderPassDefault* rpd) : RPD(rpd) { }

void RenderPassReflections::Initialise()
{
	const auto device = DX::DeviceResources::Instance()->GetD3DDevice();
	const auto outputSize = DX::DeviceResources::Instance()->GetViewportSize();

	// Create UAV
	Microsoft::WRL::ComPtr<ID3D11Texture2D> rtvTex;
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = outputSize.right;
	texDesc.Height = outputSize.bottom;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	DX::ThrowIfFailed(device->CreateTexture2D(&texDesc, nullptr, rtvTex.ReleaseAndGetAddressOf()));

	D3D11_UNORDERED_ACCESS_VIEW_DESC postProcUAVDesc;
	postProcUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	postProcUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	postProcUAVDesc.Texture2D.MipSlice = 0;
	DX::ThrowIfFailed(device->CreateUnorderedAccessView(rtvTex.Get(), &postProcUAVDesc, ResultUAV.ReleaseAndGetAddressOf()));

	// Create SRV
	Microsoft::WRL::ComPtr<ID3D11Resource> geometryPassResource;
	ResultUAV->GetResource(geometryPassResource.ReleaseAndGetAddressOf());
	DX::ThrowIfFailed(device->CreateShaderResourceView(geometryPassResource.Get(), nullptr, UnorderedAccessSRV.ReleaseAndGetAddressOf()));

	// Compile and create compute shader
	ID3DBlob* csBlob = nullptr;
	DX::ThrowIfFailed(DX::CompileShaderFromFile(L"Source/Rendering/Shaders/ReflectionShader.hlsl", "main", "cs_5_0", &csBlob));
	DX::ThrowIfFailed(device->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, ComputeShader.ReleaseAndGetAddressOf()));

	// Release blob
	csBlob->Release();
}

void RenderPassReflections::Render()
{
	const auto context = DX::DeviceResources::Instance()->GetD3DDeviceContext();
	const auto outputSize = DX::DeviceResources::Instance()->GetViewportSize();

	// Bind textures to compute shader
	const auto srvs = RPD->GetSRVs();
	for (int i = 0; i < srvs.size(); ++i)
		context->CSSetShaderResources(i, 1, &srvs[i]);
	context->CSSetUnorderedAccessViews(0, 1, ResultUAV.GetAddressOf(), nullptr);

	// Dispatch compute shader
	context->CSSetShader(ComputeShader.Get(), nullptr, 0);
	context->Dispatch(outputSize.right / 8, outputSize.bottom / 8, 1);

	// Unbind textures
	static constexpr ID3D11UnorderedAccessView* nullUav = nullptr;
	static constexpr ID3D11ShaderResourceView* nullSrv = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, &nullUav, nullptr);
	for (int i = 0; i < srvs.size(); ++i)
		context->CSSetShaderResources(i, 1, &nullSrv);
}
