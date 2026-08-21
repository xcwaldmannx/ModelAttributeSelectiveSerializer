#include "Deserializer.h"

#include <iostream>

#include <array>

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

	auto modelBoundsPos = json[BOUNDS_POS].get<std::array<float, 3>>();
	modelLayout.mBoundsPos = glm::vec3(modelBoundsPos[0], modelBoundsPos[1], modelBoundsPos[2]);

	auto modelBoundsNeg = json[BOUNDS_NEG].get<std::array<float, 3>>();
	modelLayout.mBoundsNeg = glm::vec3(modelBoundsNeg[0], modelBoundsNeg[1], modelBoundsNeg[2]);

	// get meshes
	nlohmann::json& jsonMeshes = json[MESHES];

	for (auto it = jsonMeshes.begin(); it != jsonMeshes.end(); ++it)
	{
		const std::string& name = it.key();
		nlohmann::json& meshObj = it.value();

		MeshLayout meshLayout;
		meshLayout.mName = name;

		meshLayout.mVertexOffset    = meshObj[VERTEX_OFFSET].get<uint32_t>();
		meshLayout.mVertexCount     = meshObj[VERTEX_COUNT].get<uint32_t>();
		meshLayout.mIndexOffset     = meshObj[INDEX_OFFSET].get<uint32_t>();
		meshLayout.mIndexCount      = meshObj[INDEX_COUNT].get<uint32_t>();
		meshLayout.mTransformOffset = meshObj[TRANSFORM_OFFSET].get<uint32_t>();

		auto boundsPos = meshObj[BOUNDS_POS].get<std::array<float, 3>>();
		meshLayout.mBoundsPos = glm::vec3(boundsPos[0], boundsPos[1], boundsPos[2]);

		auto boundsNeg = meshObj[BOUNDS_NEG].get<std::array<float, 3>>();
		meshLayout.mBoundsNeg = glm::vec3(boundsNeg[0], boundsNeg[1], boundsNeg[2]);

		modelLayout.mMeshLayouts.emplace_back(std::move(meshLayout));
	}

	return modelLayout;
}