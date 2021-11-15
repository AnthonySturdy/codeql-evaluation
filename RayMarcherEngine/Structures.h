#pragma once
#include "pch.h"

struct Vertex {
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT2 TexCoord;
};

struct ConstantBuffer {
	struct RenderSettings {
		unsigned int maxSteps;
		float maxDist;
		float intersectionThreshold;
		float PADDING;			// 16 bytes
	} renderSettings;

	struct WorldCamera {
		float fov;
		float position[3];		// 16 bytes
		float resolution[2];
		BOOL cameraType;
		float PADDING;			// 32 bytes
		DirectX::XMMATRIX view; // 48 Bytes
	} camera;

	struct WorldObject {
		float position[3];
		float radius;			// 16 bytes
	} object;
};