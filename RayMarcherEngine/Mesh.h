#pragma once
#include "Structures.h"

class Mesh
{
public:
	Mesh(ID3D11Device* device, ID3D11DeviceContext* context);
	Mesh(const Mesh&) = default;
	Mesh(Mesh&&) = default;
	Mesh& operator=(const Mesh&) = default;
	Mesh& operator=(Mesh&&) = default;
	~Mesh() = default;

	ID3D11Buffer* GetVertexBuffer() const { return VertexBuffer.Get(); }
	ID3D11Buffer* GetIndexBuffer() const { return IndexBuffer.Get(); }
	int GetIndexCount() const { return NumIndices; }

private:
	HRESULT InitialiseMeshData(ID3D11Device* device, ID3D11DeviceContext* context);

	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;
	int NumIndices;
};
