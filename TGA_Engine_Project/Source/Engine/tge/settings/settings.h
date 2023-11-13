#pragma once

#include <tge/engine.h>

namespace Tga {
	extern void LoadSettings(const std::string &aProjectName);
	
	namespace Settings {
		namespace Window {
			extern std::string title;
		}

		extern const EngineConfiguration &GetEngineConfiguration();

		extern std::string ResolveAssetPath(const std::string& anAsset);
		extern std::wstring ResolveAssetPathW(const std::string& anAsset);
		extern std::wstring ResolveAssetPathW(const std::wstring& anAsset);

		extern std::string ResolveEngineAssetPath(const std::string& anAsset);
		extern std::wstring ResolveEngineAssetPathW(const std::string& anAsset);
		extern std::wstring ResolveEngineAssetPathW(const std::wstring& anAsset);

		extern std::string ResolveGameAssetPath(const std::string& anAsset);
		extern std::wstring ResolveGameAssetPathW(const std::string& anAsset);
		extern std::wstring ResolveGameAssetPathW(const std::wstring& anAsset);

		extern std::string GameSettings(const std::string& aKey);
		extern std::wstring GameSettingsW(const std::string& aKey);
		extern std::wstring GameSettingsW(const std::wstring& aKey);
	}
}