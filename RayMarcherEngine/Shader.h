#pragma once

class Shader
{
public:
	Shader(ID3D11Device* device, const WCHAR* vertexShaderPathWithoutExt, const WCHAR* pixelShaderPathWithoutExt, D3D11_INPUT_ELEMENT_DESC* vertexLayout, UINT numElements);
	Shader(const Shader&) = default;
	Shader(Shader&&) = default;
	Shader& operator=(const Shader&) = default;
	Shader& operator=(Shader&&) = default;
	~Shader() = default;

	void Initialise();
	void CompilePixelShader();

	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader() { return m_VertexShader; }
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader() { return m_PixelShader; }
	Microsoft::WRL::ComPtr<ID3D11InputLayout> GetVertexLayout() { return m_VertexLayout; }

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_VertexLayout;

	ID3D11Device* Device;
	const WCHAR* VertexShaderPathWithoutExt;
	const WCHAR* PixelShaderPathWithoutExt;
	D3D11_INPUT_ELEMENT_DESC* VertexLayout;
	UINT NumElements;

	HRESULT CompileShaderFromFile(const WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blobOut);
};
