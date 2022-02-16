#include "pch.h"
#include "Scene.h"
#include "Structures.h"
#include "SDFShaderGenerator.h"

Scene::Scene(ID3D11Device* device)
{
	GameObjects.resize(OBJECT_COUNT);
	for (int i = 0; i < GameObjects.size(); i++)
		GameObjects[i].SetPosition(DirectX::SimpleMath::Vector4(i, 0.0f, 0.0f, 0.0f));

	GameObjects[0].SetObjectType(1);
	GameObjects[4].SetObjectType(1);

	CreateConstantBuffers(device);
}

void Scene::Update(float dt, ID3D11Device* device) { }

void Scene::Render(ID3D11DeviceContext* context)
{
	// Update constant buffers
	static ConstantBuffer::WorldObject objects[OBJECT_COUNT];
	static int objectTypeSum = -1;
	int curObjectTypeSum = 0;

	ImGui::Begin("Scene Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	for (int i = 0; i < GameObjects.size(); i++)
	{
		ImGui::Columns(2);

		GameObjects[i].RenderGUIControls();
		curObjectTypeSum += GameObjects[i].GetObjectType();

		ImGui::NextColumn();
		if (ImGui::Button(std::format("Remove##{0}", i).c_str()))
		{
			GameObjects.erase(GameObjects.begin() + i);
			objects[i] = ConstantBuffer::WorldObject();
			objectTypeSum = -1;
			break;
		}
		ImGui::NextColumn();
		objects[i] = GameObjects[i];
	}

	if (objectTypeSum != curObjectTypeSum)
	{
		std::string sdfs = "";
		for (const auto& go : GameObjects)
		{
			std::string goSdf = SDFShaderGenerator::GenerateSignedDistanceFunction(go.GetObjectType());
			if (!sdfs.contains(goSdf))
				sdfs += goSdf;
		}

		SDFShaderGenerator::WriteToHeaderShader(sdfs);

		SDFShaderGenerator::WriteDistanceFunctionToShader(SDFShaderGenerator::GetSceneDistanceFunctionContents(GameObjects));

		ActiveShader->CompilePixelShader();
	}
	objectTypeSum = curObjectTypeSum;

	if (GameObjects.size() != OBJECT_COUNT &&
		ImGui::Button("Add GameObject"))
	{
		GameObjects.push_back(GameObject());
		objectTypeSum = -1;
	}
	ImGui::End();

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
