#include "pch.h"
#include "GameObject.h"

void GameObject::RenderGUIControls()
{
	ImGui::PushID(this);

	if (ImGui::CollapsingHeader("GameObject"))
	{
		ImGui::DragFloat3("Position", &Position.x, 0.05f);
		ImGui::DragFloat3("Rotation", &Rotation.x, 0.05f);
		ImGui::DragFloat3("Scale", &Scale.x, 0.05f);

		ImGui::InputInt("Object Type", &ObjectType);

		ImGui::DragFloat3("Parameters", &Parameters.x, 0.05f);
	}

	ImGui::PopID();
}
