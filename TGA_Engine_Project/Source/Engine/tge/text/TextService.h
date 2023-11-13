/*
This class handles all texts that will be rendered, don't use this to show text, only use the Text class
*/

#pragma once

#include <tge/EngineDefines.h>
#include <tge/math/color.h>
#include <tge/text/fontfile.h>
#include <tge/text/text.h>
#include <unordered_map>
#include <vector>

namespace Tga
{
	class Texture;
	class Text;
	class TextService
	{
	public:
		TextService();
		~TextService();

		void Init();

		Font GetOrLoad(std::wstring aFontPathAndName, unsigned char aBorderSize, int aSize);
		bool Draw(Tga::Text& aText, Tga::SpriteShader* aCustomShaderToRenderWith = nullptr);
		float GetSentenceWidth(Tga::Text& aText);
		float GetSentenceHeight(Tga::Text& aText);

	private:
		struct FT_LibraryRec_* myLibrary;

		std::unordered_map<std::wstring, std::weak_ptr<InternalTextAndFontData>> myFontData;
	};
}