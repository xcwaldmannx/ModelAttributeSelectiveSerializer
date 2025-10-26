#include "ModelAttributeSelectiveSerializer.h"

#include "FileHandler/FileHandler.h"
#include "Serializer.h"

using namespace mass;

void ModelAttributeSelectiveSerializer::serialize(
	const Configuration& config,
	const std::string& inputFilepath,
	const std::string& outputFilepath)
{
	FileHandler fileHandler;
	Model model = fileHandler.read(config, inputFilepath);

	Serializer serializer;
	serializer.serialize(config, model, outputFilepath);
}