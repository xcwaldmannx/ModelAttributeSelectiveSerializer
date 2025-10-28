#include "../main/Mass.h"

#include <filesystem>
#include <iostream>

#include <string>

namespace fs = std::filesystem;

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

	std::string inputPath = "C:/Users/xcwal/source/repos/CrossPlatformGameEngine/CrossPlatformGameEngine/res/blender/";
	std::string outputPath = "C:/Users/xcwal/source/repos/CrossPlatformGameEngine/CrossPlatformGameEngine/res/models/";

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
		prism,
		shapes,
		windmill
	};

	for (auto& pair : pairs)
	{
		mass::serialize(config, pair.first, pair.second);

	}

	// mass::deserialize(config, outputFilepath);
}

int main()
{
	runTest();

	return 0;
}