#include "Mass.h"

#include "FileHandler/ModelHandler.h"
#include "Serialization/Serializer.h"
#include "Serialization/Deserializer.h"

namespace mass
{

	void serialize(
		const Configuration& config,
		const std::string& inputFilepath,
		const std::string& outputFilepath)
	{
		ModelHandler fileHandler;
		Model model = fileHandler.readModel(config, inputFilepath);

		Serializer serializer;
		serializer.serialize(config, model, outputFilepath);
	}

	ModelLayout deserialize(
		const Configuration& config,
		const std::string& inputFilepath)
	{
		ModelHandler fileHandler;
		nlohmann::json json = fileHandler.readSerialized(config, inputFilepath);

		Deserializer deserializer;
		return deserializer.deserialize(config, json);
	}

}
