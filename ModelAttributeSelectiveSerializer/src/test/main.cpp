#include <filesystem>
#include <iostream>

#include <string>

#include "../main/Configuration.h"
#include "../main/Importer.h"
#include "../main/Exporter.h"
#include "../main/Serializer.h"
#include "../main/Deserializer.h"

#include "../main/ModelObject.h"

namespace fs = std::filesystem;

void loadFile(const std::string& filepath, Model* model, uint32_t flags)
{
	Importer importer;
	importer.load(filepath, model, flags);
}

void serializeModel(Model* model, std::string* data)
{
	Serializer serializer;
	serializer.serialize(model, data);
}

void exportModel(const std::string& outputFilename, const std::string& outputFilepath, const std::string& data)
{
	Exporter exporter;
	exporter.save(outputFilename, outputFilepath, data);
}

ModelObject deserializeModel(const std::string& outputFilename, const std::string& outputFilepath)
{
	Deserializer deserializer;
	return deserializer.deserialize(outputFilename, outputFilepath);
}

void runTest()
{
	std::string filepath;
	std::cout << "Enter a model filepath:\n";
	std::cin >> filepath;
	if (!std::filesystem::exists(std::filesystem::path(filepath)))
	{
		throw std::runtime_error("Bad filepath.");
	}

	std::string outputFilename = "test.model";
	std::string outputFilepath = "./";

	Model model;
	uint32_t flags = HAS_NORMALS | HAS_TEXCOORDS | HAS_TRANSFORMS;

	loadFile(filepath, &model, flags);

	std::string data;

	serializeModel(&model, &data);

	exportModel(outputFilename, outputFilepath, data);

	ModelObject modelobj = deserializeModel(outputFilename, outputFilepath);

	std::cout << "Done. Close Window.";
}

void runTestConfig()
{
	Configuration config;
	config.mSizePosition = TypeSize::UINT32;
	config.mSizeNormal   = TypeSize::UINT32;
	config.mHasNormals    = true;
	config.mHasColors     = false;
	config.mHasTexCoords  = true;
	config.mHasTransforms = true;
	config.mHasAnimations = false;

	std::string inputFilepath  = "C:/Users/xcwal/Documents/Models/testmodel0.fbx";
	std::string outputFilepath = "./";
	std::string outputFilename = "testmodel.model";
}

int main()
{
	runTest();

	return 0;
}