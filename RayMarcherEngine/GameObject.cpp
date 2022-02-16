#include "pch.h"
#include "GameObject.h"

void GameObject::RenderGUIControls()
{
	ImGui::Begin("Scene Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::PushID(this);

	ImGui::DragFloat3("Position", &Position.x, 0.05f);

	ImGui::PopID();
	ImGui::End();
}
