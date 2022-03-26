#pragma once
#include "Game/GameObject.h"

class RayMarchObjectComponent : public Component
{
public:
	RayMarchObjectComponent() = default;
	RayMarchObjectComponent(const RayMarchObjectComponent&) = default;
	RayMarchObjectComponent(RayMarchObjectComponent&&) = default;
	RayMarchObjectComponent& operator=(const RayMarchObjectComponent&) = default;
	RayMarchObjectComponent& operator=(RayMarchObjectComponent&&) = default;
	~RayMarchObjectComponent() override = default;

	void Update(float deltaTime) override {};
	void Render() override {};
	void RenderGUI() override;

protected:
	[[nodiscard]] std::string GetComponentName() const override { return "Ray March Object"; }

private:
	unsigned int BoolOperator{ 0u };
	unsigned int SDFType{ 0u };
	DirectX::SimpleMath::Vector3 Position{ DirectX::SimpleMath::Vector3::Zero }; // TODO: Remove this - use transform
	DirectX::SimpleMath::Vector3 Parameters{ DirectX::SimpleMath::Vector3::One };
};
