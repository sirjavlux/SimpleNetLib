#include <stdafx.h>
#include "settings.h"

#include <nlohmann/json.hpp>
#include <fstream>

#include <io.h>
#define access _access_s

namespace Tga {
	namespace Settings {
		struct Path {
			std::string absolute;
			std::string relative;
		};
		static Path engine_assets_path;
		static Path game_assets_path;
		static nlohmann::json game_settings;

		static EngineConfiguration window_params;
	}
}

std::string Tga::Settings::Window::title;

const Tga::EngineConfiguration& Tga::Settings::GetEngineConfiguration() {
	return window_params;
}

std::string Tga::Settings::ResolveAssetPath(const std::string& anAsset) {
	if (access(anAsset.c_str(), 0) == 0) {
		return anAsset;
	}
	else if (access(("data/" + anAsset).c_str(), 0) == 0) {
		return ("data/" + anAsset);
	}

	else if (access((game_assets_path.relative + anAsset).c_str(), 0) == 0) {
		return game_assets_path.relative + anAsset;
	}
	else if (access((engine_assets_path.relative + anAsset).c_str(), 0) == 0) {
		return engine_assets_path.relative + anAsset;
	}
	else if (access((game_assets_path.absolute + anAsset).c_str(), 0) == 0) {
		return game_assets_path.absolute + anAsset;
	}
	else if (access((engine_assets_path.absolute + anAsset).c_str(), 0) == 0) {
		return engine_assets_path.absolute + anAsset;
	}
	return engine_assets_path.relative + "Sprites/error.dds";
}

std::wstring Tga::Settings::ResolveAssetPathW(const std::string& anAsset) {
	if (access(anAsset.c_str(), 0) == 0) {
		return ToWstring(anAsset);
	}
	else if (access(("data/" + anAsset).c_str(), 0) == 0) {
		return (L"data/" + ToWstring(anAsset));
	}
	else if (access((game_assets_path.relative + anAsset).c_str(), 0) == 0) {
		return ToWstring(game_assets_path.relative + anAsset);
	}
	else if (access((engine_assets_path.relative + anAsset).c_str(), 0) == 0) {
		return ToWstring(engine_assets_path.relative + anAsset);
	}
	else if (access((game_assets_path.absolute + anAsset).c_str(), 0) == 0) {
		return ToWstring(game_assets_path.absolute + anAsset);
	}
	else if (access((engine_assets_path.absolute + anAsset).c_str(), 0) == 0) {
		return ToWstring(engine_assets_path.absolute + anAsset);
	}
	return ToWstring(engine_assets_path.relative + "Sprites/error.dds");
}

std::wstring Tga::Settings::ResolveAssetPathW(const std::wstring& anAsset) {
	std::string asset = FromWstring(anAsset);
	
	if (access(asset.c_str(), 0) == 0) {
		return anAsset;
	}
	else if (access(("data/" + asset).c_str(), 0) == 0) {
		return (L"data/" + anAsset);
	}
	if (access((game_assets_path.relative + asset).c_str(), 0) == 0) {
		return ToWstring(game_assets_path.relative + asset);
	}
	else if (access((engine_assets_path.relative + asset).c_str(), 0) == 0) {
		return ToWstring(engine_assets_path.relative + asset);
	}
	else if (access((game_assets_path.absolute + asset).c_str(), 0) == 0) {
		return ToWstring(game_assets_path.absolute + asset);
	}
	else if (access((engine_assets_path.absolute + asset).c_str(), 0) == 0) {
		return ToWstring(engine_assets_path.absolute + asset);
	}
	return ToWstring(engine_assets_path.relative + "Sprites/error.dds");
}

std::string Tga::Settings::ResolveEngineAssetPath(const std::string& anAsset) {
	if (access(anAsset.c_str(), 0) == 0) {
		return anAsset;
	}
	if (access((engine_assets_path.relative + anAsset).c_str(), 0) == 0) {
		return std::string(engine_assets_path.relative + anAsset);
	}
	return std::string(engine_assets_path.absolute + anAsset);
}

std::wstring Tga::Settings::ResolveEngineAssetPathW(const std::string& anAsset) {
	if (access(anAsset.c_str(), 0) == 0) {
		return Tga::ToWstring(anAsset);
	}
	if (access((engine_assets_path.relative + anAsset).c_str(), 0) == 0) {
		return std::wstring(ToWstring(engine_assets_path.relative + anAsset));
	}
	return std::wstring(ToWstring(engine_assets_path.absolute) + ToWstring(anAsset));
}

std::wstring Tga::Settings::ResolveEngineAssetPathW(const std::wstring& anAsset) {
	std::string asset = FromWstring(anAsset);

	if (access(asset.c_str(), 0) == 0) {
		return anAsset;
	}
	if (access((game_assets_path.relative + FromWstring(anAsset)).c_str(), 0) == 0) {
		return std::wstring(ToWstring(engine_assets_path.relative) + anAsset);
	}
	return std::wstring(ToWstring(engine_assets_path.absolute) + anAsset);
}

std::string Tga::Settings::ResolveGameAssetPath(const std::string& anAsset) {
	if (access(anAsset.c_str(), 0) == 0) {
		return anAsset;
	}
	if (access((game_assets_path.relative + anAsset).c_str(), 0) == 0) {
		return std::string(game_assets_path.relative + anAsset);
	}
	return std::string(game_assets_path.absolute + anAsset);
}

std::wstring Tga::Settings::ResolveGameAssetPathW(const std::string& anAsset) {
	if (access(anAsset.c_str(), 0) == 0) {
		return Tga::ToWstring(anAsset);
	}
	if (access((game_assets_path.relative + anAsset).c_str(), 0) == 0) {
		return Tga::ToWstring(game_assets_path.relative + anAsset);
	}
	return Tga::ToWstring(game_assets_path.absolute + anAsset);
}
std::wstring Tga::Settings::ResolveGameAssetPathW(const std::wstring& anAsset) 
{
	std::string asset = FromWstring(anAsset);
	if (access(asset.c_str(), 0) == 0) {
		return anAsset;
	}
	if (access((game_assets_path.relative + asset).c_str(), 0) == 0) {
		return std::wstring(Tga::ToWstring(game_assets_path.relative) + anAsset);
	}
	return std::wstring(Tga::ToWstring(game_assets_path.absolute) + anAsset);
}

std::string Tga::Settings::GameSettings(const std::string& aKey) {
	return game_settings[aKey];
}
std::wstring Tga::Settings::GameSettingsW(const std::string& aKey) {
	std::string s = game_settings[aKey];
	return Tga::ToWstring(game_settings[aKey]);
}
std::wstring Tga::Settings::GameSettingsW(const std::wstring& aKey) {
	std::string s = game_settings[Tga::FromWstring(aKey)];
	return Tga::ToWstring(s);
}

void Tga::LoadSettings(const std::string& aProjectName) {
	using namespace Settings;
	{	
		using namespace nlohmann;
					
		/////////////////////
		// Game settings
		std::string filename = (aProjectName.find(".") == std::string::npos) ? (aProjectName + ".json").c_str() : aProjectName.c_str();
		std::string path("settings/");

		std::ifstream game_ifs(path + filename);
		assert(game_ifs);

		game_ifs >> Settings::game_settings;
		game_ifs.close();
		if (game_settings.contains("assets_path")) {
			engine_assets_path.absolute = game_settings["assets_path"]["engine"]["absolute"];
			engine_assets_path.relative = game_settings["assets_path"]["engine"]["relative"];
			game_assets_path.absolute = game_settings["assets_path"]["game"]["absolute"];
			game_assets_path.relative = game_settings["assets_path"]["game"]["relative"];
		}

		///////////////////////
		// Read create params

		//////////////////////////////////////
		// Window Title
		{
			auto & app		= game_settings["window_settings"]["title"];
			if (!app.is_null()) {
				Settings::window_params.myApplicationName = Tga::ToWstring(app);
			}
		}
		/////////////////////////////////////
		// Clear Color
		{
			auto& app		= game_settings["window_settings"]["clear_color"];
			if (!app.is_null()) {
				Settings::window_params.myClearColor = Tga::Color(app["r"], app["g"], app["b"], app["a"]);
			}
		}
		/////////////////////////////////////
		// Window Width & Height
		{
			auto& window = game_settings["window_settings"]["window_size"];
			auto& render = game_settings["window_settings"]["render_size"];
			auto& target = game_settings["window_settings"]["target_size"];

			if (window.is_null() == false) {
				Settings::window_params.myWindowSize.x = window["w"];
				Settings::window_params.myWindowSize.y = window["h"];
				if (render.is_null()) {
					Settings::window_params.myRenderSize.x = window["w"];
					Settings::window_params.myRenderSize.y = window["h"];
				}
				else {
					Settings::window_params.myRenderSize.x = render["w"];
					Settings::window_params.myRenderSize.y = render["h"];
				}
			}
			else if (render.is_null() == false) {
				Settings::window_params.myRenderSize.x = render["w"];
				Settings::window_params.myRenderSize.y = render["h"];
				Settings::window_params.myWindowSize.x = render["w"];
				Settings::window_params.myWindowSize.y = render["h"];
			}
		
			if (target.is_null() == false) {
				Settings::window_params.myTargetSize.x = target["w"];
				Settings::window_params.myTargetSize.y = target["h"];
			}
			else {
				Settings::window_params.myTargetSize.x = 0;
				Settings::window_params.myTargetSize.y = 0;
			}
		}
		//////////////////////////////////////
		// VSync
		{
			auto& app = game_settings["enable_vsync"];
			if (!app.is_null()) {
				Settings::window_params.myEnableVSync = app;
			}
		}
		/////////////////////////////////////
		// Start in fullscreen
		{
			auto& app = game_settings["window_settings"]["start_in_fullscreen"];
			if (!app.is_null()) {
				Settings::window_params.myStartInFullScreen = app;
			}
		}
		/////////////////////////////////////
		// Letterbox and pillarbox
		{
			auto& app = game_settings["window_settings"]["use_letterbox_and_pillarbox"];
			if (!app.is_null()) {
				Settings::window_params.myUseLetterboxAndPillarbox = app;
			}
		}
	}
}