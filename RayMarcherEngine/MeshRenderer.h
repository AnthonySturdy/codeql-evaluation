#pragma once
#include "Structures.h"
#include "Shader.h"
#include "Mesh.h"

class MeshRenderer
{
public:
	MeshRenderer() = default;
	MeshRenderer(const MeshRenderer&) = default;
	MeshRenderer(MeshRenderer&&) = default;
	MeshRenderer& operator=(const MeshRenderer&) = default;
	MeshRenderer& operator=(MeshRenderer&&) = default;
	~MeshRenderer() = default;

	void InitMesh(ID3D11Device* device, ID3D11DeviceContext* context);
	void InitShader(ID3D11Device* device, const WCHAR* vertexShaderPathWithoutExt, const WCHAR* pixelShaderPathWithoutExt, D3D11_INPUT_ELEMENT_DESC* vertexLayout, UINT numElements);

	void Update(float t, ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context);

	void RenderGUIControls(ID3D11Device* device);

	std::shared_ptr<Shader> GetShader() { return MeshShader; }

private:
	std::shared_ptr<Shader> MeshShader;
	std::shared_ptr<Mesh> MeshData;
};
