#pragma once

#include "ModelLoader.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <string>

#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

class ModelExporter
{
public:
	void exportModel(std::string filename, std::string filepath, Model* model)
	{
		nlohmann::json exportData;
		exportData["version"] = "1.0.0";
		exportData["filename"] = filename;
		exportData["filepath"] = fs::absolute(fs::path(filepath));

		exportData["flags"] =
		{
			{ "HAS_NORMALS",    (model->mFlags & HAS_NORMALS)    != 0 },
			{ "HAS_COLORS",     (model->mFlags & HAS_COLORS)     != 0 },
			{ "HAS_TEXCOORDS",  (model->mFlags & HAS_TEXCOORDS)  != 0 },
			{ "HAS_TRANSFORMS", (model->mFlags & HAS_TRANSFORMS) != 0 },
			{ "HAS_ANIMATIONS", (model->mFlags & HAS_ANIMATIONS) != 0 }
		};

		exportData["vertexStride"] = model->mVertexStrideBytes;

		exportData["vertexCount"] = model->mVertices.size();
		exportData["indexCount"] = model->mIndices.size();
		exportData["transformCount"] = model->mTransforms.size();

		exportData["vertices"] = nlohmann::json::array();

		for (unsigned int vIndex = 0; vIndex < model->mVertices.size(); vIndex++)
		{
			auto& vertex = model->mVertices[vIndex];
			auto& position = vertex.mPosition;

			exportData["vertices"][vIndex].push_back(position.x);
			exportData["vertices"][vIndex].push_back(position.y);
			exportData["vertices"][vIndex].push_back(position.z);

			if (model->mFlags & HAS_NORMALS)
			{
				auto& normal = vertex.mNormal;
				exportData["vertices"][vIndex].push_back(normal.x);
				exportData["vertices"][vIndex].push_back(normal.y);
				exportData["vertices"][vIndex].push_back(normal.z);
			}

			if (model->mFlags & HAS_COLORS)
			{
				auto& color = vertex.mColor;
				exportData["vertices"][vIndex].push_back(color.x);
				exportData["vertices"][vIndex].push_back(color.y);
				exportData["vertices"][vIndex].push_back(color.z);
				exportData["vertices"][vIndex].push_back(color.z);
			}

			if (model->mFlags & HAS_TEXCOORDS)
			{
				auto& texcoord = vertex.mTexCoord;
				exportData["vertices"][vIndex].push_back(texcoord.x);
				exportData["vertices"][vIndex].push_back(texcoord.y);
			}
		}

		exportData["indices"] = model->mIndices;

		if (model->mFlags & HAS_TRANSFORMS)
		{
			exportData["transforms"] = nlohmann::json::array();

			for (unsigned int transform = 0; transform < model->mTransforms.size(); transform++)
			{
				auto& trans = model->mTransforms[transform];
				nlohmann::json t = nlohmann::json::array();

				for (int c = 0; c < 4; ++c) {
					for (int r = 0; r < 4; ++r) {
						t.push_back(trans[c][r]);
					}
				}

				exportData["transforms"].push_back(t);
			}
		}

		if (model->mFlags & HAS_ANIMATIONS)
		{
			// TODO: serialize animations
		}

		// write to file
		fs::path path = fs::path(filepath) / fs::path(filename);
		std::ofstream ofs(path);
		if (!ofs)
		{
			throw std::runtime_error("Could not open output file!");
		}

		ofs << exportData.dump();
		ofs.close();

		std::cout << "Wrote file to " << path << ".\n";
	}
};