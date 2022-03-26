#include "pch.h"
#include "RayMarchObjectComponent.h"

void RayMarchObjectComponent::RenderGUI()
{
	//ImGui::DragFloat3("Position", &Position.x, 0.05f);
	//ImGui::DragFloat3("Rotation", &Rotation.x, 0.05f);
	//ImGui::DragFloat3("Scale", &Scale.x, 0.05f);

	int sdfType = SDFType; // ImGui::InputInt doesn't accept unsigned int
	ImGui::InputInt("Object Type", &sdfType);
	SDFType = std::max(0, sdfType);

	ImGui::DragFloat3("Parameters", &Parameters.x, 0.05f);

	// TODO: Implement boolean operator selector
}
