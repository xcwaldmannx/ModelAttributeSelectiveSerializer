#pragma once

#include "../Configuration.h"
#include "../Model/Model.h"

#include <string>

#include <assimp/scene.h>

namespace mass
{

	class FileHandler
	{
	public:
		Model read(const Configuration& config, const std::string& filepath);
		void write(const std::string& data, const std::string& filepath);

	private:
		void processNodes(aiNode* node, aiMesh** meshes, Model& model, const Configuration& config);
	};

}
