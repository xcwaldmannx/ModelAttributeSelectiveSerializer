#include "Mass.h"

#include "FileHandler/FileHandler.h"
#include "Serialization/Serializer.h"
#include "Serialization/Deserializer.h"

namespace mass
{

	void serialize(
		const Configuration& config,
		const std::string& inputFilepath,
		const std::string& outputFilepath)
	{
		FileHandler fileHandler;
		Model model = fileHandler.readModel(config, inputFilepath);

		Serializer serializer;
		serializer.serialize(config, model, outputFilepath);
	}

	ModelLayout deserialize(
		const Configuration& config,
		const std::string& inputFilepath)
	{
		FileHandler fileHandler;
		nlohmann::json json = fileHandler.readSerialized(config, inputFilepath);

		Deserializer deserializer;
		return deserializer.deserialize(config, json);
	}

}
