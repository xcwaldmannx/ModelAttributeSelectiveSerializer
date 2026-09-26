#include "Serializer.h"

#include "FileHandler/ModelHandler.h"

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

	json[BOUNDS_POS] = { model.mBoundsPos.x, model.mBoundsPos.y, model.mBoundsPos.z };
	json[BOUNDS_NEG] = { model.mBoundsNeg.x, model.mBoundsNeg.y, model.mBoundsNeg.z };

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

	for (auto &[name, mesh] : model.mMeshes)
	{
		json[MESHES][name] = nlohmann::json::object();

		auto& namedMesh = json[MESHES][name];

		namedMesh[VERTEX_OFFSET]    = mesh.mVertexOffset;
		namedMesh[VERTEX_COUNT]     = mesh.mVertexCount;
		namedMesh[INDEX_OFFSET]     = mesh.mIndexOffset;
		namedMesh[INDEX_COUNT]      = mesh.mIndexCount;
		namedMesh[TRANSFORM_OFFSET] = mesh.mTransformOffset;
		namedMesh[BOUNDS_POS]       = { mesh.mBoundsPos.x, mesh.mBoundsPos.y, mesh.mBoundsPos.z };
		namedMesh[BOUNDS_NEG]       = { mesh.mBoundsNeg.x, mesh.mBoundsNeg.y, mesh.mBoundsNeg.z };
	}

	const std::string data = json.dump(2);

	ModelHandler fileHandler;
	fileHandler.write(data, outputFilepath);
}