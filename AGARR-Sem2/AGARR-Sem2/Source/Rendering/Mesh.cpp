#include "pch.h"
#include "Mesh.h"

using namespace DirectX::SimpleMath;

Mesh::Mesh()
{
	Vertices =
	{
		{Vector3(-1.0f, -1.0f, 0.0f), Vector3::Up, Vector2(0.0f, 1.0f)},
		{Vector3(1.0f, -1.0f, 0.0f), Vector3::Up, Vector2(1.0f, 1.0f)},
		{Vector3(1.0f, 1.0f, 0.0f), Vector3::Up, Vector2(1.0f, 0.0f)},
		{Vector3(-1.0f, 1.0f, 0.0f), Vector3::Up, Vector2(0.0f, 0.0f)}
	};

	Indices =
	{
		3, 1, 0,
		2, 1, 3,
	};

	const auto device = DX::DeviceResources::Instance()->GetD3DDevice();

	// Create vertex buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * Vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = Vertices.data();

	DX::ThrowIfFailed(device->CreateBuffer(&bd, &InitData, VertexBuffer.ReleaseAndGetAddressOf()));

	// Create index buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(unsigned short) * Indices.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	InitData.pSysMem = Indices.data();

	DX::ThrowIfFailed(device->CreateBuffer(&bd, &InitData, IndexBuffer.ReleaseAndGetAddressOf()));
}
