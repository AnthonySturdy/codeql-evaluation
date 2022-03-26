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
	DX::ThrowIfFailed(CompileShaderFromFile(L"Source/Rendering/Shaders/VertexShader.hlsl", "main", "vs_5_0", &vsBlob));
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
	DX::ThrowIfFailed(CompileShaderFromFile(L"Source/Rendering/Shaders/PixelShader.hlsl", "main", "ps_5_0", &psBlob));
	DX::ThrowIfFailed(device->CreatePixelShader(psBlob->GetBufferPointer(),
	                                            psBlob->GetBufferSize(),
	                                            nullptr,
	                                            PixelShader.ReleaseAndGetAddressOf()));

	// Release blobs
	psBlob->Release();
}

HRESULT Shader::CompileShaderFromFile(const WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Improves shader debugging experience but still allows for optimisation.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging.
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DCompileFromFile(fileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, shaderModel,
	                        dwShaderFlags, 0, blobOut, &pErrorBlob);

	if (FAILED(hr))
	{
		// Output if D3DCompileFromFile has error
		if (pErrorBlob)
		{
			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			pErrorBlob->Release();
		}

		return hr;
	}

	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}
