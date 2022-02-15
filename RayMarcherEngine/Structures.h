#pragma once
#include "pch.h"

#define OBJECT_COUNT 10

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT2 TexCoord;
};

struct ConstantBuffer
{
	struct RenderSettings
	{
		RenderSettings() : maxSteps(300), maxDist(500.0f), intersectionThreshold(0.01f) {}

		unsigned int maxSteps;
		float maxDist;
		float intersectionThreshold;
		float PADDING{}; // 16 bytes
	} renderSettings;

	struct WorldCamera
	{
		float fov;
		float position[3]; // 16 bytes
		unsigned int resolution[2];
		BOOL cameraType;
		float PADDING; // 32 bytes
		DirectX::XMMATRIX view; // 48 Bytes
	} camera;

	struct WorldObject
	{
		WorldObject() : isActive(false), position{0.0f, 0.0f, 0.0f}, objectType(0), params{0.0f, 0.0f, 0.0f} {}

		BOOL isActive;
		float position[3]; // 16 bytes
		unsigned int objectType;
		float params[3]; // 32 bytes
	} object[OBJECT_COUNT];
};
