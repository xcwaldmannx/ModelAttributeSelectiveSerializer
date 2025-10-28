#include "../main/Mass.h"

#include <filesystem>
#include <iostream>

#include <string>

using namespace mass;

namespace fs = std::filesystem;

int main()
{
	// get model filepath
	std::string inputFilepath;
	std::cout << "Enter a filepath to a model:\n";
	std::getline(std::cin, inputFilepath);

	if (!fs::exists(inputFilepath))
	{
		std::cout << "Filepath does not exist!\n";
		return 0;
	}

	// get output filepath
	std::string outputFilepath;
	std::cout << "Enter an output filepath:\n";
	std::getline(std::cin, outputFilepath);

	if (!fs::exists(outputFilepath))
	{
		std::cout << "Filepath does not exist!\n";
		return 0;
	}

	// get model attributes
	std::string attributes;
	std::cout <<
		"[1] HAS_NORMALS\n"    <<
		"[2] HAS_COLORS\n"     <<
		"[3] HAS_TEXCOORDS\n"  <<
		"[4] HAS_TRANSFORMS\n" <<
		"[5] HAS_ANIMATIONS\n" <<
		"Select attributes for this model (space separated):\n";

	std::getline(std::cin, attributes);

	std::istringstream iss(attributes);
	std::string token;

	Configuration config{};
	config.mVertexLayout.mAttributes =
	{
		{ 3, sizeof(float), 0 },
		{ 3, sizeof(float), sizeof(float) * 3 },
		{ 2, sizeof(float), sizeof(float) * 6 }
	};
	config.mVertexLayout.mStride = sizeof(float) * 8;

	while (iss >> token) {
		try {
			int choice = std::stoi(token);
			switch (choice) {
			case 1: config.mHasNormals    = true; break;
			case 2: config.mHasColors     = true; break;
			case 3: config.mHasTexCoords  = true; break;
			case 4: config.mHasTransforms = true; break;
			case 5: config.mHasAnimations = true; break;
			default:
				std::cout << "Unknown option: " << choice << "\n";
			}
		}
		catch (const std::invalid_argument&) {
			std::cout << "Invalid input (not a number): " << token << "\n";
			return 0;
		}
	}

	mass::serialize(config, inputFilepath, outputFilepath);

	return 0;
}