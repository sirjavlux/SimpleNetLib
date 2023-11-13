----------------------------------------------------------------------------
-- the dirs table is a listing of absolute paths, since we generate projects
-- and files it makes a lot of sense to make them absolute to avoid problems
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
dirs = {}
dirs["root"] 			= os.realpath("../")
dirs["bin"]				= os.realpath(dirs.root .. "Bin/")
dirs["temp"]			= os.realpath(dirs.root .. "Temp/")
dirs["lib"]				= os.realpath(dirs.root .. "Lib/")
dirs["projectfiles"]	= os.realpath(dirs.root .. "Local/")
dirs["source"] 			= os.realpath(dirs.root .. "Source/")
dirs["dependencies"]	= os.realpath(dirs.root .. "Dependencies/")
dirs["game"]			= os.realpath(dirs.root .. "Source/Game/")
dirs["enhetstest"]		= os.realpath(dirs.root .. "Source/EnhetsTest/")
dirs["server"]			= os.realpath(dirs.root .. "Source/Server/")
dirs["client"]			= os.realpath(dirs.root .. "Source/Client/")
dirs["external"]		= os.realpath(dirs.root .. "Source/External/")
dirs["engine"]			= os.realpath(dirs.root .. "Source/Engine")
dirs["settings"]		= os.realpath(dirs.root .. "Bin/settings/")
dirs["engine_assets"] 	= os.realpath(dirs.root .. "EngineAssets/")
dirs["proxyserver"]     = os.realpath(dirs.root .. "Source/ProxyServer/")
dirs["clientsimulation"]= os.realpath(dirs.root .. "Source/ClientSimulation/")
dirs["simplenetapi"]	= os.realpath(dirs.root .. "../SimpleNetAPI/SimpleNetAPI/")

dirs["shaders"]	= {}
dirs.shaders["absolute"] = os.realpath(dirs.root .. "Bin/Shaders/")
dirs.shaders["relative"] = "../Bin/Shaders/"

engine_settings = os.realpath(dirs.settings .. "/EngineSettings.json")


-----------------------------------------------------------------------
-- These should be more or less equivalent with Engines WindowConfiguration struct
-- Some of it can't be set like this, things like callbacks.
function default_settings()
	return {
		assets_path = {
			engine = {
				absolute = path.translate(dirs.engine_assets, "/"),
				relative = path.getrelative(dirs.bin, dirs.engine_assets) .. "/"
			},
			game = {
				absolute = path.translate(os.realpath("./data/"), "/"),
				relative = path.getrelative(dirs.bin, os.realpath("./data/")) .. "/"
			}
		},

		window_settings = {
			window_size = {w=1600, h=900},
			render_size = {w=1600, h=900},
			target_size = {w=1600, h=900},
			title = "TGE - Never give up on your dreams!",
	 		clear_color = {r=0.0, g=0.2, b=0.25, a=1.0},

			use_letterbox_and_pillarbox = false,
			keep_aspect_ratio = true,
			aspect_ratio = 1.7,

	 		start_in_fullscreen = false
		},
		enable_vsync = true,
	}
end

if not os.isdir (dirs.bin) then
	os.mkdir (dirs.bin)
end

if not os.isdir(dirs.settings) then 
	os.mkdir (dirs.settings)
end

---------------------------------------------------------------------------
-- Utility function to create individual project-settings any game project, 
-- tutorial or similar is responsible for running:
-- Tga::LoadSettings("game_name.json"); at startup
function verify_or_create_settings(game_name)
	local settings_filename = game_name .. ".json"
	defines { 'TGE_PROJECT_SETTINGS_FILE="' .. settings_filename .. '"' }
	local game_settings = dirs["settings"] .. settings_filename
	
	local settings = default_settings()
	if os.isfile(game_settings) then
		local old_settings = json.decode(io.readfile(game_settings))
		for k,v in pairs(old_settings) do
			settings[k] = v
		end

		settings.assets_path.engine.absolute = path.translate(dirs.engine_assets, "/")
		settings.assets_path.game.absolute = path.translate(os.realpath("./data/"), "/")
	end

	io.writefile(
		game_settings,
		json.encode(settings)
	)
end