#pragma once
#include "Game/GameObject.h"
#include "Game/Components/RayMarchObjectComponent.h"

#define RAYMARCH_MAX_OBJECTS 30

class RayMarchingManagerComponent : public Component
{
	struct RenderSettings
	{
		unsigned int Resolution[2]{ 0u, 0u };
		unsigned int MaxSteps{ 300u };
		float MaxDist{ 500.0f };
		float IntersectionThreshold{ 0.01f };

		float PADDING[3]{};
	};

	struct RayMarchScene
	{
		struct Object
		{
			DirectX::SimpleMath::Vector3 Position{ DirectX::SimpleMath::Vector4::Zero };
			float pW{ 0.0f };
			DirectX::SimpleMath::Vector3 Rotation{ DirectX::SimpleMath::Vector4::Zero };
			float rW{ 0.0f };
			DirectX::SimpleMath::Vector3 Scale{ DirectX::SimpleMath::Vector4::One };
			float sW{ 0.0f };
			DirectX::SimpleMath::Vector3 Parameters{ DirectX::SimpleMath::Vector3::One };
			unsigned int SDFType{ 0u };
			unsigned int BoolOperator{ 0u };

			float PADDING[3]{};
		} ObjectsList[RAYMARCH_MAX_OBJECTS];
	};

public:
	RayMarchingManagerComponent(const std::vector<GameObject*>& gameObjects);
	RayMarchingManagerComponent(const RayMarchingManagerComponent&) = default;
	RayMarchingManagerComponent(RayMarchingManagerComponent&&) = default;
	RayMarchingManagerComponent& operator=(const RayMarchingManagerComponent&) = default;
	RayMarchingManagerComponent& operator=(RayMarchingManagerComponent&&) = default;
	~RayMarchingManagerComponent() override = default;

	void Update(float deltaTime) override;
	void Render() override;
	void RenderGUI() override;

protected:
	[[nodiscard]] std::string GetComponentName() const override { return "Ray Marching Manager"; }

private:
	void CreateConstantBuffers();

	RenderSettings RenderSettingsData{};
	RayMarchScene RayMarchSceneData{};
	Microsoft::WRL::ComPtr<ID3D11Buffer> RenderSettingsConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> RayMarchSceneConstantBuffer;

	const std::vector<GameObject*>& GameObjects;
};