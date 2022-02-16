#pragma once
#include "Structures.h"

class GameObject
{
public:
	GameObject() = default;
	GameObject(const GameObject&) = default;
	GameObject(GameObject&&) = default;
	GameObject& operator=(const GameObject&) = default;
	GameObject& operator=(GameObject&&) = default;
	~GameObject() = default;

	void RenderGUIControls();

	operator ConstantBuffer::WorldObject() const
	{
		ConstantBuffer::WorldObject obj;
		obj.isActive = true;

		obj.position[0] = Position.x;
		obj.position[1] = Position.y;
		obj.position[2] = Position.z;

		obj.objectType = 0;

		obj.params[0] = 1.0f;

		return obj;
	}

	void SetPosition(DirectX::SimpleMath::Vector4 position) { Position = position; }

private:
	DirectX::SimpleMath::Vector4 Position{ DirectX::SimpleMath::Vector4::Zero };
	DirectX::SimpleMath::Vector4 Rotation{ DirectX::SimpleMath::Vector4::Zero };
	DirectX::SimpleMath::Vector4 Scale{ DirectX::SimpleMath::Vector4::One };
};
