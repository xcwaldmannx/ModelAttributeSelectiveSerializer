#include "Exporter.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace fs = std::filesystem;

void Exporter::save(const std::string& filename, const std::string& filepath, const std::string& data)
{
	fs::path path = fs::path(filepath) / fs::path(filename);
	std::ofstream ofs(path);
	if (!ofs)
	{
		throw std::runtime_error("Could not open output file!");
	}

	ofs << data;
	ofs.close();

	std::cout << "Wrote file to " << path << ".\n";
}