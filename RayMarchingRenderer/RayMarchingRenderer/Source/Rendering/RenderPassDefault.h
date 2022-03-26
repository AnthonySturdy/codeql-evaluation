#pragma once
#include "Rendering/RenderPass.h"

class GameObject;

class RenderPassDefault : public RenderPass
{
public:
	RenderPassDefault(std::vector<GameObject*>& gameObjects);
	RenderPassDefault(const RenderPassDefault&) = default;
	RenderPassDefault(RenderPassDefault&&) = default;
	RenderPassDefault& operator=(const RenderPassDefault&) = delete;
	RenderPassDefault& operator=(RenderPassDefault&&) = delete;
	~RenderPassDefault() override = default;

	void Initialise() override;
	void Render() override;
	void RenderGUI() override;

	[[nodiscard]] ID3D11ShaderResourceView* GetSRV() const { return RenderTargetSRV.Get(); }

private:
	std::vector<GameObject*>& GameObjects;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> RenderState;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> RenderTargetSRV;
};
