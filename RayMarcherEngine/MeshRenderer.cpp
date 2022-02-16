#include "pch.h"
#include "MeshRenderer.h"

using namespace DirectX;

void MeshRenderer::InitMesh(ID3D11Device* device, ID3D11DeviceContext* context)
{
	MeshData = std::make_shared<Mesh>(device, context);

	// Set primitive topology
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void MeshRenderer::InitShader(ID3D11Device* device, const WCHAR* vertexShaderPathWithoutExt, const WCHAR* pixelShaderPathWithoutExt, D3D11_INPUT_ELEMENT_DESC* vertexLayout, UINT numElements)
{
	MeshShader = std::make_shared<Shader>(device, vertexShaderPathWithoutExt, pixelShaderPathWithoutExt, vertexLayout, numElements);
}

void MeshRenderer::Render(ID3D11DeviceContext* context)
{
	ID3D11Buffer* vertexBuf = MeshData->GetVertexBuffer();
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertexBuf, &stride, &offset);
	context->IASetIndexBuffer(MeshData->GetIndexBuffer(), DXGI_FORMAT_R16_UINT, 0);

	context->DrawIndexed(MeshData->GetIndexCount(), 0, 0);
}

void MeshRenderer::RenderGUIControls(ID3D11Device* device) { }
