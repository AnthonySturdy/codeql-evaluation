#include "pch.h"
#include "Shader.h"

#ifdef _MSC_VER
#pragma comment(lib, "d3dcompiler") // Automatically link with d3dcompiler.lib as we are using D3DCompileFromFile() below.
#endif

Shader::Shader()
{
	CreateVertexShaderAndInputLayout();
	CreatePixelShader();
}

void Shader::CreateVertexShaderAndInputLayout()
{
	const auto device = DX::DeviceResources::Instance()->GetD3DDevice();

	// Read and create Vertex shader
	ID3DBlob* vsBlob = nullptr;
	DX::ThrowIfFailed(DX::CompileShaderFromFile(L"Source/Rendering/Shaders/VertexShader.hlsl", "main", "vs_5_0", &vsBlob));
	DX::ThrowIfFailed(device->CreateVertexShader(vsBlob->GetBufferPointer(),
	                                             vsBlob->GetBufferSize(),
	                                             nullptr,
	                                             VertexShader.ReleaseAndGetAddressOf()));

	// Create the input layout
	DX::ThrowIfFailed(device->CreateInputLayout(InputLayoutDesc,
	                                            InLayoutNumElements,
	                                            vsBlob->GetBufferPointer(),
	                                            vsBlob->GetBufferSize(),
	                                            InputLayout.ReleaseAndGetAddressOf()));

	vsBlob->Release();
}

void Shader::CreatePixelShader()
{
	const auto device = DX::DeviceResources::Instance()->GetD3DDevice();

	// Read and create Pixel shader
	ID3DBlob* psBlob = nullptr;
	DX::ThrowIfFailed(DX::CompileShaderFromFile(L"Source/Rendering/Shaders/PixelShader.hlsl", "main", "ps_5_0", &psBlob));
	DX::ThrowIfFailed(device->CreatePixelShader(psBlob->GetBufferPointer(),
	                                            psBlob->GetBufferSize(),
	                                            nullptr,
	                                            PixelShader.ReleaseAndGetAddressOf()));

	// Release blobs
	psBlob->Release();
}
