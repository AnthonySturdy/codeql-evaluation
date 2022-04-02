#pragma once
#include "Game/GameObject.h"

class MaterialComponent : public Component
{
public:
	MaterialComponent() = default;
	MaterialComponent(const MaterialComponent&) = default;
	MaterialComponent(MaterialComponent&&) = default;
	MaterialComponent& operator=(const MaterialComponent&) = default;
	MaterialComponent& operator=(MaterialComponent&&) = default;
	~MaterialComponent() override = default;

	void Update(float deltaTime) override {};
	void Render() override {};
	void RenderGUI() override;

	[[nodiscard]] DirectX::SimpleMath::Vector3 GetColour() const { return Colour; }
	[[nodiscard]] float GetMetalicness() const { return Metalicness; }
	[[nodiscard]] float GetRoughness() const { return Roughness; }

protected:
	[[nodiscard]] std::string GetComponentName() const override { return "Material"; }

private:
	DirectX::SimpleMath::Vector3 Colour{ DirectX::SimpleMath::Vector3::One };
	float Metalicness{ 0.0f };
	float Roughness{ 0.0f };
};
