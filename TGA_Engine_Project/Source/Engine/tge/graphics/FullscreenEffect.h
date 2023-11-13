#pragma once
#include <array>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct ID3D11VertexShader;
struct ID3D11PixelShader;

namespace Tga
{

class FullscreenEffect
{
public:
	virtual ~FullscreenEffect() = default;
	FullscreenEffect();
	virtual bool Init(const char* aPixelShaderPath);
	virtual void Render();
private:
	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11PixelShader> myPixelShader;
};

} // namespace Tga