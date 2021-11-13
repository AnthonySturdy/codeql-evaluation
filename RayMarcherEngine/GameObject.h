#pragma once
#include "Structures.h"
#include "Shader.h"
#include "Mesh.h"

#include <vector>

class GameObject {
public:
	GameObject();
	~GameObject();

	void InitMesh(ID3D11Device* device, ID3D11DeviceContext* context);
	void InitShader(ID3D11Device* device, const WCHAR* vertexShaderPathWithoutExt, const WCHAR* pixelShaderPathWithoutExt, D3D11_INPUT_ELEMENT_DESC* vertexLayout, UINT numElements);

	void Update(float t, ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context);

	void RenderGUIControls(ID3D11Device* device);

	DirectX::XMFLOAT4X4* GetTransform() { return &m_world; }
	std::shared_ptr<Shader> GetShader() { return m_shader; }

	void SetPosition(DirectX::XMFLOAT3 pos) { m_position = pos; }

private:
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_rotation;
	DirectX::XMFLOAT3 m_scale;
	DirectX::XMFLOAT4X4 m_world;

	std::shared_ptr<Shader> m_shader;
	std::shared_ptr<Mesh> m_mesh;
};
