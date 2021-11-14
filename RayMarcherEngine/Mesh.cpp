#include "pch.h"
#include "Mesh.h"

using namespace DirectX;

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* context) {
	InitialiseMeshData(device, context);
}

Mesh::~Mesh() {

}

HRESULT Mesh::InitialiseMeshData(ID3D11Device* device, ID3D11DeviceContext* context) {
	// Create vertices and indices
	std::vector<Vertex> vertices =
	{
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }
	};

	std::vector<WORD> indices =
	{
		3,1,0,
		2,1,3,
	};

	// Create vertex buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = vertices.data();
	HRESULT hr = device->CreateBuffer(&bd, &InitData, &m_vertexBuffer);
	if (FAILED(hr))
		return hr;

	// Create index buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * indices.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices.data();
	hr = device->CreateBuffer(&bd, &InitData, &m_indexBuffer);
	if (FAILED(hr))
		return hr;

	return S_OK;
}