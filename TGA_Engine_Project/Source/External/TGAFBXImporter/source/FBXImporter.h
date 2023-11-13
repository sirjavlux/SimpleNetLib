#pragma once
#include <cstring>
#include <string>

#include "FBXImporterStructs.h"

namespace TGA
{
	struct FBXAnimation;
	struct FBXModel;

	class FBXImporter
	{
		inline static std::string myLastError;

	public:

		/**
		 * Attempts to load an FBX model into the provided FBXModel structure. Supports skeletal meshes, multiple meshes
		 * and multiple materials.
		 * NOTE: The FBXModel structure is very barebones but contain enough data to generate your own ModelData :).
		 * @param someFilePath The FBX file to load.
		 * @param outModel The model data read from the file.
		 * @param ignoreNoMesh If true LoadModel will return if it managed to load no meshes from the file. Useful to load i.e. a skeleton.
		 * @returns True if the file was read successfully and a model loaded.
		 */
		static bool LoadModel(const std::string& someFilePath, FBXModel& outModel);

		/**
		 * Checks the specified file to see if it contains a model with at least one mesh.
		 * @param someFilePath The FBX file to load.
		 * @returns True if there is a mesh in the file.
		 */
		static bool IsValidModelFile(const std::string& someFilePath);

		/**
		 * Attempts to load an animation from an FBX file into the provided outAnimation. Does not require mesh in the file.
		 * NOTE: The FBXAnimation structure is very barebones but contains enough information to play an animation
		 * on a model with blending. You may, however, want to generate your own structure!
		 * @param someFilePath The FBX file to load.
		 * @param BoneNames A list of all bone names in the skeleton, for finding their frames transforms.
		 * @param outAnimation The animation data read from the file.
		 * @returns True if the file was read successfully and an animation loaded.
		 */
		static bool LoadAnimation(const std::string& someFilePath, const std::vector<std::string>& BoneNames, FBXAnimation& outAnimation);

		/**
		 * Attempts to load a skeleton only from the specified file.
		 * @param someFilePath The FBX file to load.
		 * @param outModel The resulting model that should have a skeleton loaded.
		 * @returns True if the file was read successfully and a skeleton loaded.
		 */
		static bool LoadSkeleton(const std::string& someFilePath, FBXModel& outModel);

		/**
		 * Instructs the importer to destruct itself. It can no longer be used after this!
		 * Intended to be called at the end of the program.
		 */
		static void UnloadImporter();

		/**
		 * Retrieves the last error message from the FBX SDK, if there is one.
		 * @returns A string containing the error message.
		 */
		static __forceinline const std::string& GetLastError() { return myLastError; }
	};
}
