#include "Deserializer.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

ModelObject Deserializer::deserialize(const std::string& filename, const std::string& filepath)
{
	fs::path path = fs::path(filepath) / fs::path(filename);
	std::ifstream ifs(path);
	if (!ifs)
	{
		throw std::runtime_error("Could not read file!");
	}

	nlohmann::json json = nlohmann::json::parse(ifs);

	ifs.close();

	ModelObject modelobj;

	nlohmann::json& flags = json["flags"];

	if (flags["HAS_NORMALS"] == true)
	{
		std::cout << "has normals\n";
	}

	if (flags["HAS_COLORS"] == true)
	{
		std::cout << "has colors\n";
	}

	if (flags["HAS_TEXCOORDS"] == true)
	{
		std::cout << "has texcoords\n";
	}

	if (flags["HAS_TRANSFORMS"] == true)
	{
		std::cout << "has transforms\n";
	}

	if (flags["HAS_ANMIATIONS"] == true)
	{
		std::cout << "has animations\n";
	}

	return modelobj;
}