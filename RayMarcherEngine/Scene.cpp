#include "pch.h"
#include "Scene.h"
#include "Structures.h"
#include "SDFGenerator.h"

Scene::Scene(ID3D11Device* device)
{
	GameObjects.resize(OBJECT_COUNT);
	for (int i = 0; i < GameObjects.size(); i++)
		GameObjects[i].SetPosition(DirectX::SimpleMath::Vector4(i, 0.0f, 0.0f, 0.0f));

	CreateConstantBuffers(device);
}

void Scene::Update(float dt, ID3D11Device* device) { }

void Scene::Render(ID3D11DeviceContext* context)
{
	// Update constant buffers
	static ConstantBuffer::WorldObject objects[OBJECT_COUNT];
	for (int i = 0; i < GameObjects.size(); i++)
	{
		GameObjects[i].RenderGUIControls();
		objects[i] = GameObjects[i];
	}

	context->UpdateSubresource(GameObjectsConstantBuffer.Get(), 0, nullptr, &objects, 0, 0);
	context->PSSetConstantBuffers(2, 1, GameObjectsConstantBuffer.GetAddressOf());
}

void Scene::CreateConstantBuffers(ID3D11Device* device)
{
	// Create the constant buffers
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer::WorldObject) * OBJECT_COUNT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, GameObjectsConstantBuffer.GetAddressOf());
}
