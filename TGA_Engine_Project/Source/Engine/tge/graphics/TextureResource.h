#pragma once

#include <tge/Math/Vector.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct ID3D11ShaderResourceView;

namespace Tga
{

class TextureResource
{
protected:
	ComPtr<ID3D11ShaderResourceView> mySRV;

public:
	TextureResource();
	TextureResource(ID3D11ShaderResourceView* aSRV);
	~TextureResource();

	void SetAsResourceOnSlot(unsigned int aSlot) const;
	ID3D11ShaderResourceView* GetShaderResourceView() const { return mySRV.Get(); };
	void SetShaderResourceView(ID3D11ShaderResourceView* aSRV);
	Vector2ui CalculateTextureSize() const;
};

} // namespace Tga