#pragma once
#include "Game/GameObject.h"

class RayMarchLightComponent : public Component
{
public:
	RayMarchLightComponent() = default;
	RayMarchLightComponent(const RayMarchLightComponent&) = default;
	RayMarchLightComponent(RayMarchLightComponent&&) = default;
	RayMarchLightComponent& operator=(const RayMarchLightComponent&) = default;
	RayMarchLightComponent& operator=(RayMarchLightComponent&&) = default;
	~RayMarchLightComponent() override = default;

	void Update(float deltaTime) override { };
	void Render() override { };
	void RenderGUI() override;

	[[nodiscard]] DirectX::SimpleMath::Vector3 GetColour() const { return Colour; }
	[[nodiscard]] float GetConstantAttenuation() const { return ConstantAttenuation; }
	[[nodiscard]] float GetLinearAttenuation() const { return LinearAttenuation; }
	[[nodiscard]] float GetQuadraticAttenuation() const { return QuadraticAttenuation; }

protected:
	[[nodiscard]] std::string GetComponentName() const override { return "Ray March Light"; }

private:
	DirectX::SimpleMath::Vector3 Colour{ DirectX::SimpleMath::Vector3::One };
	float ConstantAttenuation{ 0.5f };
	float LinearAttenuation{ 0.1f };
	float QuadraticAttenuation{ 0.01f };
};
