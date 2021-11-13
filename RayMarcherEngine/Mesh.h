#pragma once
#include <vector>
#include "Structures.h"

class Mesh {
public:
	Mesh(ID3D11Device* device, ID3D11DeviceContext* context);
	~Mesh();

	ID3D11Buffer* GetVertexBuffer() const { return m_vertexBuffer.Get();}
	ID3D11Buffer* GetIndexBuffer() const { return m_indexBuffer.Get(); } 

private:
	HRESULT InitialiseMeshData(ID3D11Device* device, ID3D11DeviceContext* context);

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
};