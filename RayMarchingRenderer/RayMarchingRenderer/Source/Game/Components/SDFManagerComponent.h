#pragma once
#include "Game/GameObject.h"
#include "RayMarchObjectComponent.h"

#include <filesystem>


class SDFManagerComponent : public Component
{
public:
	SDFManagerComponent() = default;
	SDFManagerComponent(const SDFManagerComponent&) = default;
	SDFManagerComponent(SDFManagerComponent&&) = default;
	SDFManagerComponent& operator=(const SDFManagerComponent&) = default;
	SDFManagerComponent& operator=(SDFManagerComponent&&) = default;
	~SDFManagerComponent() override = default;

	void Update(float deltaTime) override {};
	void Render() override {};
	void RenderGUI() override;

	[[nodiscard]] std::string GenerateSignedDistanceFunction(int objectType) const;
	[[nodiscard]] std::string GenerateSceneDistanceFunctionContents(const std::vector<RayMarchObjectComponent*>& gameObjects) const;

	void WriteStringToHeaderShader(std::string content, std::ios_base::openmode writeMode = std::ios_base::out) const;
	void WriteSceneDistanceFunctionToShaderHeader(const std::string& funcContents) const;

protected:
	[[nodiscard]] std::string GetComponentName() const override { return "SDF Manager"; }

private:
	std::vector<std::pair<std::string, std::string>> SDFFuncContents = {
		{
			"Sphere",
			"return length(p) - param.x;"
		},
		{
			"Box",
			"float3 q = abs(p) - param.xyz; return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);"
		},
		{
			"Torus",
			"float2 q = float2(length(p.xz) - param.x, p.y); return length(q) - param.y;"
		},
		{
			"Cone",
			R"(float2 q = param.z * float2(param.x / param.y, -1.0);
    float2 w = float2(length(p.xz), p.y);
    float2 a = w - q * clamp(dot(w, q) / dot(q, q), 0.0, 1.0);
    float2 b = w - q * float2(clamp(w.x / q.x, 0.0, 1.0), 1.0);
    float k = sign(q.y);
    float d = min(dot(a, a), dot(b, b));
    float s = max(k * (w.x * q.y - w.y * q.x), k * (w.y - q.y));
    return sqrt(d) * sign(s);)"
		},
		{
			"Cylinder",
			"float2 d = abs(float2(length(p.xz), p.y)) - float2(param.x, param.y); return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));"
		}
	};

	const std::filesystem::path ShaderHeaderPath = std::filesystem::current_path() / "Source" / "Rendering" / "Shaders" / "GeneratedSceneDistance.hlsli";
	const std::filesystem::path ShaderHeaderTemplatePath = std::filesystem::current_path() / "Source" / "Rendering" / "Shaders" / "SceneDistanceTemplate.hlsli";
	const std::string DistanceFunctionContentsFlag = "$DIST_FUNC_CONTENTS";
};
