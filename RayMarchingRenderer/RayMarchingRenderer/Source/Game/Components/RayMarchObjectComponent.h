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

	[[nodiscard]] int GetBoolOperator() const { return BoolOperator; }
	[[nodiscard]] int GetSDFType() const { return SDFType; }
	[[nodiscard]] DirectX::SimpleMath::Vector3 GetParameters() const { return Parameters; }

protected:
	[[nodiscard]] std::string GetComponentName() const override { return "Ray March Object"; }

private:
	int BoolOperator{ 0 };
	int SDFType{ 0 };
	DirectX::SimpleMath::Vector3 Parameters{ DirectX::SimpleMath::Vector3::One };
};
