#include <filesystem>
#include <iostream>

#include <string>

#include "../main/ModelAttributeSelectiveSerializer.h"

namespace fs = std::filesystem;

void runTest()
{
	mass::Configuration config{};
	config.mVertexStride.mPosition = mass::TypeSize::VEC3;
	config.mVertexStride.mNormal   = mass::TypeSize::VEC3;
	config.mVertexStride.mTexCoord = mass::TypeSize::VEC2;
	config.mHasNormals    = true;
	config.mHasColors     = false;
	config.mHasTexCoords  = true;
	config.mHasTransforms = true;
	config.mHasAnimations = false;

	std::string inputFilepath  = "C:/Users/xcwal/Documents/Models/testmodel0.fbx";
	std::string outputFilepath = "./testmodel.model";

	mass::ModelAttributeSelectiveSerializer mass;
	mass.serialize(config, inputFilepath, outputFilepath);
}

int main()
{
	runTest();

	return 0;
}