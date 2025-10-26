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
	json["version"] = "1.0.0";

	json["flags"] = nlohmann::json::object();
	json["flags"].push_back({ "hasNormals",    (config.mHasNormals)    ? true : false });
	json["flags"].push_back({ "hasColors",     (config.mHasColors)     ? true : false });
	json["flags"].push_back({ "hasTexCoords",  (config.mHasTexCoords)  ? true : false });
	json["flags"].push_back({ "hasTransforms", (config.mHasTransforms) ? true : false });
	json["flags"].push_back({ "hasAnimations", (config.mHasAnimations) ? true : false });

	json["vertexStride"] = config.mVertexStride.size();

	json["vertexCount"] = model.mVertices.size();
	json["indexCount"] = model.mIndices.size();
	json["transformCount"] = model.mTransforms.size();
	json["animationCount"] = model.mAnimations.size();

	json["vertices"] = nlohmann::json::array();

	for (unsigned int vIndex = 0; vIndex < model.mVertices.size(); vIndex++)
	{
		auto& vertex = model.mVertices[vIndex];
		auto& position = vertex.mPosition;

		json["vertices"][vIndex].push_back(position.x);
		json["vertices"][vIndex].push_back(position.y);
		json["vertices"][vIndex].push_back(position.z);

		if (config.mHasNormals)
		{
			auto& normal = vertex.mNormal;
			json["vertices"][vIndex].push_back(normal.x);
			json["vertices"][vIndex].push_back(normal.y);
			json["vertices"][vIndex].push_back(normal.z);
		}

		if (config.mHasColors)
		{
			auto& color = vertex.mColor;
			json["vertices"][vIndex].push_back(color.x);
			json["vertices"][vIndex].push_back(color.y);
			json["vertices"][vIndex].push_back(color.z);
			json["vertices"][vIndex].push_back(color.w);
		}

		if (config.mHasTexCoords)
		{
			auto& texcoord = vertex.mTexCoord;
			json["vertices"][vIndex].push_back(texcoord.x);
			json["vertices"][vIndex].push_back(texcoord.y);
		}
	}

	json["indices"] = model.mIndices;

	if (config.mHasTransforms)
	{
		json["transforms"] = nlohmann::json::array();

		for (unsigned int transform = 0; transform < model.mTransforms.size(); transform++)
		{
			auto& trans = model.mTransforms[transform];
			nlohmann::json t = nlohmann::json::array();

			for (int c = 0; c < 4; ++c) {
				for (int r = 0; r < 4; ++r) {
					t.push_back(trans[c][r]);
				}
			}

			json["transforms"].push_back(t);
		}
	}

	if (config.mHasAnimations)
	{
		// TODO: serialize animations
	}

	// get sub-mesh data
	json["meshes"] = nlohmann::json::object();

	for (auto meshes = model.mMeshes.begin(); meshes != model.mMeshes.end(); meshes++)
	{
		std::string name = meshes->first;
		const Mesh& mesh = meshes->second;

		json["meshes"][name] =
		{
			mesh.mVertexOffset,
			mesh.mVertexCount,
			mesh.mIndexOffset,
			mesh.mIndexCount,
			mesh.mTransformOffset
		};
	}

	std::string data = json.dump(2);

	FileHandler fileHandler;
	fileHandler.write(data, outputFilepath);
}