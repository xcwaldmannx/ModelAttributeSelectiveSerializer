#include "Deserializer.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace fs = std::filesystem;

void Deserializer::deserialize(const std::string& filename, const std::string& filepath, nlohmann::json* json)
{
	fs::path path = fs::path(filepath) / fs::path(filename);
	std::ifstream ifs(path);
	if (!ifs)
	{
		throw std::runtime_error("Could not read file!");
	}

	*json = nlohmann::json::parse(ifs);

	ifs.close();
}