#include "Deserializer.h"

#include <iostream>

using namespace mass;

ModelLayout Deserializer::deserialize(const Configuration& config, nlohmann::json& json)
{
	// validate the json

	// check version
	if (json[VERSION].get<std::string>() != API_VERSION)
	{
		throw std::runtime_error("Versions do not match!");
	}

	// check flags
	nlohmann::json& flags = json[FLAGS];

	if (!(config.mHasNormals    == flags[HAS_NORMALS]    &&
		  config.mHasColors     == flags[HAS_COLORS]     &&
		  config.mHasTexCoords  == flags[HAS_TEXCOORDS]  &&
		  config.mHasTransforms == flags[HAS_TRANSFORMS] &&
		  config.mHasAnimations == flags[HAS_ANIMATIONS]))
	{
		throw std::runtime_error("Bad configuration!");
	}


	VertexLayout vertexLayout;

	nlohmann::json& jsonLayout = json[VERTEX_LAYOUT];

	vertexLayout.mStride = json[VERTEX_STRIDE].get<size_t>();

	for (auto& attribute : jsonLayout)
	{
		vertexLayout.mAttributes.push_back(
			{
				attribute[0].get<size_t>(),
				attribute[1].get<size_t>(),
				attribute[2].get<size_t>(),
			}
		);
	}

	ModelLayout modelLayout;
	modelLayout.mVertexLayout = vertexLayout;
	modelLayout.mVertices = json[VERTICES].get<std::vector<float>>();
	modelLayout.mIndices = json[INDICES].get<std::vector<uint32_t>>();

	return modelLayout;
}