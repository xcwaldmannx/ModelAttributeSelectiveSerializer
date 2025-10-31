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

	// get vertex layout
	VertexLayout vertexLayout;

	nlohmann::json& jsonLayout = json[VERTEX_LAYOUT];

	vertexLayout.mStride = json[VERTEX_STRIDE].get<uint32_t>();

	for (auto& attribute : jsonLayout)
	{
		vertexLayout.mAttributes.push_back(
			{
				attribute[0].get<uint32_t>(),
				attribute[1].get<uint32_t>(),
				attribute[2].get<uint32_t>(),
			}
		);
	}

	// get vertices, indices and transforms
	ModelLayout modelLayout;
	modelLayout.mVertexLayout = vertexLayout;
	modelLayout.mVertices = json[VERTICES].get<std::vector<float>>();
	modelLayout.mIndices = json[INDICES].get<std::vector<uint32_t>>();
	modelLayout.mTransforms = json[TRANSFORMS].get<std::vector<float>>();

	// get meshes
	nlohmann::json& jsonMeshes = json[MESHES];

	for (auto& mesh : jsonMeshes)
	{
		MeshLayout meshLayout;
		meshLayout.mName = mesh[NAME].get<std::string>();
		meshLayout.mVertexOffset    = mesh[NAME][VERTEX_OFFSET].get<uint32_t>();
		meshLayout.mVertexCount     = mesh[NAME][VERTEX_COUNT].get<uint32_t>();
		meshLayout.mIndexOffset     = mesh[NAME][INDEX_OFFSET].get<uint32_t>();
		meshLayout.mIndexCount      = mesh[NAME][INDEX_COUNT].get<uint32_t>();
		meshLayout.mTransformOffset = mesh[NAME][TRANSFORM_OFFSET].get<uint32_t>();
		modelLayout.mMeshLayouts.emplace_back(std::move(meshLayout));
	}

	return modelLayout;
}