#include "pch.h"
#include "RayMarchLightComponent.h"

void RayMarchLightComponent::RenderGUI()
{
	ImGui::ColorEdit3("Colour", &Colour.x);
	ImGui::DragFloat("Shadow Sharpness", &ShadowSharpness, 0.1f);
	ImGui::DragFloat("Constant Attenuation", &ConstantAttenuation, 0.001f);
	ImGui::DragFloat("Linear Attenuation", &LinearAttenuation, 0.001f);
	ImGui::DragFloat("Quadratic Attenuation", &QuadraticAttenuation, 0.001f);
}
