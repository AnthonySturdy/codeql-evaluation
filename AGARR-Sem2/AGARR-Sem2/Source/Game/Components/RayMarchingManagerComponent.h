#pragma once
#include "Game/GameObject.h"

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

public:
	RayMarchingManagerComponent();
	RayMarchingManagerComponent(const RayMarchingManagerComponent&) = default;
	RayMarchingManagerComponent(RayMarchingManagerComponent&&) = default;
	RayMarchingManagerComponent& operator=(const RayMarchingManagerComponent&) = default;
	RayMarchingManagerComponent& operator=(RayMarchingManagerComponent&&) = default;
	~RayMarchingManagerComponent() override = default;

	void Update(float deltaTime) override {};
	void Render() override;
	void RenderGUI() override;

protected:
	[[nodiscard]] std::string GetComponentName() const override { return "Ray Marching Manager"; }

private:
	void CreateConstantBuffer();
	RenderSettings RenderSettingsData;
	Microsoft::WRL::ComPtr<ID3D11Buffer> RenderSettingsConstantBuffer;
};
