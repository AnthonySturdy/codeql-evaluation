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

		obj.objectType = ObjectType;

		obj.params[0] = Parameters.x;
		obj.params[1] = Parameters.y;
		obj.params[2] = Parameters.z;

		return obj;
	}

	void SetPosition(DirectX::SimpleMath::Vector4 position) { Position = position; }
	void SetRotation(DirectX::SimpleMath::Vector4 rotation) { Rotation = rotation; }
	void SetScale(DirectX::SimpleMath::Vector4 scale) { Scale = scale; }
	void SetObjectType(int type) { ObjectType = type; }

	int GetObjectType() const { return ObjectType; }

private:
	DirectX::SimpleMath::Vector4 Position{ DirectX::SimpleMath::Vector4::Zero };
	DirectX::SimpleMath::Vector4 Rotation{ DirectX::SimpleMath::Vector4::Zero };
	DirectX::SimpleMath::Vector4 Scale{ DirectX::SimpleMath::Vector4::One };
	int ObjectType = 0;
	DirectX::SimpleMath::Vector3 Parameters{ DirectX::SimpleMath::Vector3::One };
};
