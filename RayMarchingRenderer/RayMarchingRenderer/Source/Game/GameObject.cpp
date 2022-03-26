#include "pch.h"
#include "Game/GameObject.h"

#include <format>
#include <string>

#include "Game/Components/TransformComponent.h"

GameObject::GameObject()
{
	AddTransform();
}

GameObject::GameObject(const std::string name) : Name(name)
{
	AddTransform();
}

void GameObject::AddTransform()
{
	if (!GetComponent<TransformComponent>())
	{
		auto* comp = AddComponent(new TransformComponent());
		comp->Removable = false;
	}
}

void GameObject::Update(float deltaTime)
{
	for (const auto& [type, components] : Components)
		for (const auto& component : components)
			component->Update(deltaTime);
}

void GameObject::Render()
{
	for (const auto& [type, components] : Components)
		for (const auto& component : components)
			component->Render();
}

void GameObject::RenderGUI()
{
	ImGui::PushID(this);

	if (ImGui::CollapsingHeader((Name + "###").c_str()))
	{
		ImGui::InputText("Name", &Name);

		for (const auto& [type, components] : Components)
		{
			for (const auto& component : components)
			{
				ImGui::PushID(component.get());
				if (ImGui::TreeNode(component->GetComponentName().c_str()))
				{
					component->RenderGUI();

					ImGui::Separator();

					ImGui::TreePop();
				}
				ImGui::PopID();
			}
		}
	}

	ImGui::PopID();
}
