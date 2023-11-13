#pragma once
#include <tge/render/RenderCommon.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct ID3D11SamplerState;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
struct ID3D11BlendState;

namespace Tga
{
	class RenderStateManager
	{
	public:
		bool Init();

		void SetBlendState(BlendState aBlendState);
		void SetDepthStencilState(DepthStencilState aDepthStencilState);
		void SetRasterizerState(RasterizerState aRasterizerState);
		void SetSamplerState(SamplerFilter aFilter, SamplerAddressMode aAddressMode);
		void SetAllStates(const RenderState& RenderState);
		void ResetStates();
	private:
		bool CreateBlendStates();
		bool CreateDepthStencilStates();
		bool CreateRasterizerStates();
		bool CreateSamplers();

		std::array<std::array<ComPtr<ID3D11SamplerState>, (int)SamplerAddressMode::Count>, (int)SamplerFilter::Count> mySamplerStates;
		std::array<ComPtr<ID3D11DepthStencilState>, (int)DepthStencilState::Count> myDepthStencilStates;
		std::array<ComPtr<ID3D11RasterizerState>, (int)RasterizerState::Count> myRasterizerStates;
		std::array<ComPtr<ID3D11BlendState>, (int)BlendState::Count> myBlendStates;
	};
}