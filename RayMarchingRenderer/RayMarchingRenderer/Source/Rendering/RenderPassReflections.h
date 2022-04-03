#pragma once
#include "RenderPass.h"
#include <Rendering\RenderPassDefault.h>

class RenderPassReflections : public RenderPass
{
public:
	RenderPassReflections(const RenderPassDefault* rpd);
	RenderPassReflections(const RenderPassReflections&) = default;
	RenderPassReflections(RenderPassReflections&&) = default;
	RenderPassReflections& operator=(const RenderPassReflections&) = delete;
	RenderPassReflections& operator=(RenderPassReflections&&) = delete;
	~RenderPassReflections() override = default;

	void Initialise() override;
	void Render() override;
	void RenderGUI() override {};

	[[nodiscard]] ID3D11ShaderResourceView* GetSRV() const { return UnorderedAccessSRV.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> ComputeShader{ nullptr };

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> ResultUAV{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> UnorderedAccessSRV{ nullptr };

	const RenderPassDefault* RPD;
};
