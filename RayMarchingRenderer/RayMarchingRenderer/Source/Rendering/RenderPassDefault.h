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

	[[nodiscard]] ID3D11ShaderResourceView* GetSRV() const { return RenderTargetSRV[0].Get(); }
	[[nodiscard]] ID3D11ShaderResourceView* GetSRV(const int i) const { return RenderTargetSRV[i % RenderTargetSRV.size()].Get(); }

	[[nodiscard]] std::vector<ID3D11ShaderResourceView*> GetSRVs() const
	{
		std::vector<ID3D11ShaderResourceView*> res;
		for (const auto& srv : RenderTargetSRV)
			res.push_back(srv.Get());
		return res;
	}

private:
	std::vector<GameObject*>& GameObjects;

	std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> RenderTargetViews{};
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView{};
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> RenderState{};

	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> RenderTargetSRV{};
};
