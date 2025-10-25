#include "Serializer.h"

#include <iostream>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

void Serializer::serialize(Model* model, std::string* data)
{
	nlohmann::json json;
	json["version"] = "1.0.0";

	json["flags"] =
	{
		{ "HAS_NORMALS",    (model->mFlags & HAS_NORMALS) != 0 },
		{ "HAS_COLORS",     (model->mFlags & HAS_COLORS) != 0 },
		{ "HAS_TEXCOORDS",  (model->mFlags & HAS_TEXCOORDS) != 0 },
		{ "HAS_TRANSFORMS", (model->mFlags & HAS_TRANSFORMS) != 0 },
		{ "HAS_ANIMATIONS", (model->mFlags & HAS_ANIMATIONS) != 0 }
	};

	json["vertexStride"] = model->mVertexStrideBytes;

	json["vertexCount"] = model->mVertices.size();
	json["indexCount"] = model->mIndices.size();
	json["transformCount"] = model->mTransforms.size();

	json["vertices"] = nlohmann::json::array();

	for (unsigned int vIndex = 0; vIndex < model->mVertices.size(); vIndex++)
	{
		auto& vertex = model->mVertices[vIndex];
		auto& position = vertex.mPosition;

		json["vertices"][vIndex].push_back(position.x);
		json["vertices"][vIndex].push_back(position.y);
		json["vertices"][vIndex].push_back(position.z);

		if (model->mFlags & HAS_NORMALS)
		{
			auto& normal = vertex.mNormal;
			json["vertices"][vIndex].push_back(normal.x);
			json["vertices"][vIndex].push_back(normal.y);
			json["vertices"][vIndex].push_back(normal.z);
		}

		if (model->mFlags & HAS_COLORS)
		{
			auto& color = vertex.mColor;
			json["vertices"][vIndex].push_back(color.x);
			json["vertices"][vIndex].push_back(color.y);
			json["vertices"][vIndex].push_back(color.z);
			json["vertices"][vIndex].push_back(color.w);
		}

		if (model->mFlags & HAS_TEXCOORDS)
		{
			auto& texcoord = vertex.mTexCoord;
			json["vertices"][vIndex].push_back(texcoord.x);
			json["vertices"][vIndex].push_back(texcoord.y);
		}
	}

	json["indices"] = model->mIndices;

	if (model->mFlags & HAS_TRANSFORMS)
	{
		json["transforms"] = nlohmann::json::array();

		for (unsigned int transform = 0; transform < model->mTransforms.size(); transform++)
		{
			auto& trans = model->mTransforms[transform];
			nlohmann::json t = nlohmann::json::array();

			for (int c = 0; c < 4; ++c) {
				for (int r = 0; r < 4; ++r) {
					t.push_back(trans[c][r]);
				}
			}

			json["transforms"].push_back(t);
		}
	}

	if (model->mFlags & HAS_ANIMATIONS)
	{
		// TODO: serialize animations
	}

	// get sub-mesh data
	json["meshes"] = nlohmann::json::object();

	for (auto meshes = model->mMeshes.begin(); meshes != model->mMeshes.end(); meshes++)
	{
		std::string name = meshes->first;
		Mesh& mesh = meshes->second;

		json["meshes"][name] =
		{
			mesh.mVertexOffset,
			mesh.mVertexCount,
			mesh.mIndexOffset,
			mesh.mIndexCount,
			mesh.mTransformOffset
		};
	}

	*data = json.dump();
}