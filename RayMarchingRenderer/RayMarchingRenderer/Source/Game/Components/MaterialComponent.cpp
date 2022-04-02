#include "pch.h"
#include "MaterialComponent.h"

void MaterialComponent::RenderGUI()
{
	ImGui::ColorEdit3("Colour", &Colour.x);
	ImGui::SliderFloat("Metalicness", &Metalicness, 0.0f, 1.0f);
	ImGui::SliderFloat("Roughness", &Roughness, 0.0f, 1.0f);
}
