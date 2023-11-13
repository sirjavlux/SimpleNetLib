#pragma once

#include <tge/Math/Vector.h>
#include <tge/Graphics/TextureResource.h>
#include <wrl/client.h>
#include <dxgiformat.h>
#include <memory>

using Microsoft::WRL::ComPtr;

struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct D3D11_VIEWPORT;
struct ID3D11Buffer;

namespace Tga
{

class DepthBuffer;

class RenderTarget : public TextureResource
{
	ComPtr<ID3D11RenderTargetView> myRenderTarget;
	std::shared_ptr<const D3D11_VIEWPORT> myViewport;

public:
	RenderTarget();
	~RenderTarget();

	static RenderTarget Create(Vector2ui aSize, DXGI_FORMAT aFormat = DXGI_FORMAT_R8G8B8A8_UNORM);
	static RenderTarget Create(ID3D11Texture2D* aTexture);

	void Clear(Vector4f aClearColor = { 0,0,0,0 });
	void SetAsActiveTarget(DepthBuffer* aDepth = nullptr);
};

} // namespace Tga