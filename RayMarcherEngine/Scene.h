#pragma once

#include "GameObject.h"
#include "Shader.h"

class Scene
{
public:
	Scene(ID3D11Device* device);
	Scene(const Scene&) = default;
	Scene(Scene&&) = default;
	Scene& operator=(const Scene&) = default;
	Scene& operator=(Scene&&) = default;
	~Scene() = default;

	void Update(float dt, ID3D11Device* device);
	void Render(ID3D11DeviceContext* context);

	void SetShader(std::shared_ptr<Shader> shader) { ActiveShader = shader; }

private:
	void CreateConstantBuffers(ID3D11Device* device);

	std::vector<GameObject> GameObjects{};

	std::shared_ptr<Shader> ActiveShader{};
	Microsoft::WRL::ComPtr<ID3D11Buffer> GameObjectsConstantBuffer;
};
