#include "pch.h"
#include "RayMarchObjectComponent.h"

void RayMarchObjectComponent::RenderGUI()
{
	ImGui::InputInt("Object Type", &SDFType);
	SDFType = std::max(0, SDFType);

	ImGui::DragFloat3("Parameters", &Parameters.x, 0.05f);

	const char* csgOptions[3] = { "Add", "Subtract", "Intersect" };
	ImGui::Combo("Bool Operation", &BoolOperator, csgOptions, 3);
}
