#pragma once
#include "pch.h"

struct SimpleVertex {
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT2 TexCoord;
};

struct ConstantBuffer {
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
	DirectX::XMFLOAT4 vOutputColor;
};