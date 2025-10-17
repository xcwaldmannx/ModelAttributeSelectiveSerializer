#include <filesystem>
#include <iostream>

#include <string>

#include "../main/Importer.h"
#include "../main/Exporter.h"
#include "../main/Serializer.h"
#include "../main/Deserializer.h"

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

void deserializeModel(const std::string& outputFilename, const std::string& outputFilepath, nlohmann::json* json)
{
	Deserializer deserializer;
	deserializer.deserialize(outputFilename, outputFilepath, json);
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

	nlohmann::json json;

	deserializeModel(outputFilename, outputFilepath, &json);

	std::cout << json.dump(2) << "\n";

	std::cout << "Done. Close Window.";
}

int main()
{
	runTest();
	return 0;
}