#include "../main/Mass.h"

#include <filesystem>
#include <iostream>

#include <string>

#define LINUX

void runTest()
{
	mass::Configuration config{};
	config.mVertexLayout.mAttributes =
	{
		{ 3, sizeof(float), 0 },
		{ 3, sizeof(float), sizeof(float) * 3 },
		{ 2, sizeof(float), sizeof(float) * 6 }
	};
	config.mVertexLayout.mStride = sizeof(float) * 8;
	config.mHasNormals    = true;
	config.mHasColors     = false;
	config.mHasTexCoords  = true;
	config.mHasTransforms = true;
	config.mHasAnimations = false;

#ifdef LINUX
	std::string inputPath = "/home/cwaldmann/Documents/BlenderModels/";
	std::string outputPath = "/home/cwaldmann/Documents/BlenderModels/";
#elifdef WINDOWS
	std::string inputPath = "C:/Users/xcwal/source/repos/CrossPlatformGameEngine/CrossPlatformGameEngine/res/blender/";
	std::string outputPath = "C:/Users/xcwal/source/repos/CrossPlatformGameEngine/CrossPlatformGameEngine/res/models/";
#endif

	std::pair<std::string, std::string> submarine =
	{
		inputPath + "submarine.fbx",
		outputPath + "submarine.model"
	};

	std::pair<std::string, std::string> test =
	{
		inputPath + "test.fbx",
		outputPath + "test.model"
	};

	std::pair<std::string, std::string> sphere =
	{
		inputPath + "sphere.fbx",
		outputPath + "sphere.model"
	};

	std::pair<std::string, std::string> prism =
	{
		inputPath + "prism.fbx",
		outputPath + "prism.model"
	};

	std::pair<std::string, std::string> shapes =
	{
		inputPath + "shapes.fbx",
		outputPath + "shapes.model"
	};

	std::pair<std::string, std::string> windmill =
	{
		inputPath + "windmill.fbx",
		outputPath + "windmill.model"
	};

	std::vector<std::pair<std::string, std::string>> pairs =
	{
		submarine,
		test,
		sphere
		//prism,
		//shapes,
		//windmill
	};

	for (auto& [in, out] : pairs)
	{
		mass::serialize(config, in, out);
		auto model = mass::deserialize(config, out);
	}
}

namespace fs = std::filesystem;

int main()
{
	runTest();

	return 0;
}
