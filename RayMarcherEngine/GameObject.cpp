#include "pch.h"
#include "GameObject.h"

using namespace DirectX;

#define NUM_INDICES 6

GameObject::GameObject() : m_position(XMFLOAT3()), m_rotation(XMFLOAT3()), m_scale(XMFLOAT3(3, 3, 3)), m_world(XMFLOAT4X4()) {
	// Initialise world matrix
	XMStoreFloat4x4(&m_world, XMMatrixIdentity());
}

GameObject::~GameObject() {
}

void GameObject::InitMesh(ID3D11Device* device, ID3D11DeviceContext* context) {
	m_mesh = std::make_shared<Mesh>(device, context);

	// Set primitive topology
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void GameObject::InitShader(ID3D11Device* device, const WCHAR* vertexShaderPathWithoutExt, const WCHAR* pixelShaderPathWithoutExt, D3D11_INPUT_ELEMENT_DESC* vertexLayout, UINT numElements) {
	m_shader = std::make_shared<Shader>(device, vertexShaderPathWithoutExt, pixelShaderPathWithoutExt, vertexLayout, numElements);
}

void GameObject::Update(float t, ID3D11DeviceContext* context) {
	static float cummulativeTime = 0;
	cummulativeTime += t;

	XMMATRIX scale = XMMatrixScalingFromVector(XMLoadFloat3(&m_scale));
	XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_rotation));
	XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&m_position));
	
	XMMATRIX world = scale * rotation * translation;
	XMStoreFloat4x4(&m_world, world);
}

void GameObject::Render(ID3D11DeviceContext* context) {
	ID3D11Buffer* vertexBuf = m_mesh->GetVertexBuffer();
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertexBuf, &stride, &offset);
	context->IASetIndexBuffer(m_mesh->GetIndexBuffer(), DXGI_FORMAT_R16_UINT, 0);

	context->DrawIndexed(NUM_INDICES, 0, 0);
}

void GameObject::RenderGUIControls(ID3D11Device* device) {
	
}