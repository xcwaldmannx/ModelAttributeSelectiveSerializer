#include "Serializer.h"

#include "FileHandler/FileHandler.h"

#include <iostream>
#include <stdexcept>

#include <nlohmann/json.hpp>

using namespace mass;

namespace fs = std::filesystem;

void Serializer::serialize(
	const Configuration& config,
	Model& model,
	const std::string& outputFilepath)
{
	nlohmann::json json;
	json[VERSION] = "1.1.0";

	json[FLAGS][HAS_NORMALS] = config.mHasNormals;
	json[FLAGS][HAS_COLORS] = config.mHasColors;
	json[FLAGS][HAS_TEXCOORDS] = config.mHasTexCoords;
	json[FLAGS][HAS_TRANSFORMS] = config.mHasTransforms;
	json[FLAGS][HAS_ANIMATIONS] = config.mHasAnimations;

	json[VERTEX_LAYOUT] = nlohmann::json::array();

	for (int i = 0; i < config.mVertexLayout.mAttributes.size(); i++)
	{
		auto& attribute = config.mVertexLayout.mAttributes[i];
		json[VERTEX_LAYOUT].push_back(
			{
				attribute.mComponentCount,
				attribute.mComponentSize,
				attribute.mOffset
			}
		);
	}

	json[VERTEX_STRIDE] = config.mVertexLayout.mStride;

	json[VERTEX_COUNT]    = model.mVertices.size();
	json[INDEX_COUNT]     = model.mIndices.size();
	json[TRANSFORM_COUNT] = model.mTransforms.size();
	json[ANIMATION_COUNT] = model.mAnimations.size();

	json[VERTICES] = nlohmann::json::array();

	for (unsigned int vIndex = 0; vIndex < model.mVertices.size(); vIndex++)
	{
		auto& vertex = model.mVertices[vIndex];
		auto& position = vertex.mPosition;

		json[VERTICES].push_back(position.x);
		json[VERTICES].push_back(position.y);
		json[VERTICES].push_back(position.z);

		if (config.mHasNormals)
		{
			auto& normal = vertex.mNormal;
			json[VERTICES].push_back(normal.x);
			json[VERTICES].push_back(normal.y);
			json[VERTICES].push_back(normal.z);
		}

		if (config.mHasColors)
		{
			auto& color = vertex.mColor;
			json[VERTICES].push_back(color.x);
			json[VERTICES].push_back(color.y);
			json[VERTICES].push_back(color.z);
			json[VERTICES].push_back(color.w);
		}

		if (config.mHasTexCoords)
		{
			auto& texcoord = vertex.mTexCoord;
			json[VERTICES].push_back(texcoord.x);
			json[VERTICES].push_back(texcoord.y);
		}
	}

	json[INDICES] = model.mIndices;

	if (config.mHasTransforms)
	{
		json[TRANSFORMS] = nlohmann::json::array();

		for (unsigned int transform = 0; transform < model.mTransforms.size(); transform++)
		{
			auto& trans = model.mTransforms[transform];

			for (int c = 0; c < 4; ++c)
			{
				for (int r = 0; r < 4; ++r)
				{
					json[TRANSFORMS].push_back(trans[c][r]);
				}
			}
		}
	}

	if (config.mHasAnimations)
	{
		// TODO: serialize animations
	}

	// get sub-mesh data
	json[MESHES] = nlohmann::json::object();

	for (auto meshes = model.mMeshes.begin(); meshes != model.mMeshes.end(); meshes++)
	{
		std::string name = meshes->first;
		const Mesh& mesh = meshes->second;

		json[MESHES][NAME] = nlohmann::json::object();

		json[MESHES][NAME][VERTEX_OFFSET]    = mesh.mVertexOffset;
		json[MESHES][NAME][VERTEX_COUNT]     = mesh.mVertexCount;
		json[MESHES][NAME][INDEX_OFFSET]     = mesh.mIndexOffset;
		json[MESHES][NAME][INDEX_COUNT]      = mesh.mIndexCount;
		json[MESHES][NAME][TRANSFORM_OFFSET] = mesh.mTransformOffset;
	}

	std::string data = json.dump(2);

	FileHandler fileHandler;
	fileHandler.write(data, outputFilepath);
}