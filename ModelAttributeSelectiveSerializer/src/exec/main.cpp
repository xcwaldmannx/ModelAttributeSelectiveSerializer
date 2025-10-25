#include <filesystem>
#include <iostream>

#include <string>

#include "../main/Importer.h"
#include "../main/Exporter.h"
#include "../main/Serializer.h"
#include "../main/Deserializer.h"

namespace fs = std::filesystem;

int main()
{
	// get model filepath
	std::string modelFilepath;
	std::cout << "Enter a filepath to a model:\n";
	std::getline(std::cin, modelFilepath);

	if (!fs::exists(modelFilepath))
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

	// get output filename
	std::string outputFilename;
	std::cout << "Enter a name for the output file:\n";
	std::getline(std::cin, outputFilename);

	if (fs::exists(outputFilename))
	{
		std::cout << "File already exists!\n";
		return 0;
	}

	// get model attributes
	std::string attributes;
	std::cout <<
		"[1] HAS_NORMALS\n" <<
		"[2] HAS_COLORS\n" <<
		"[3] HAS_TEXCOORDS\n" <<
		"[4] HAS_TRANSFORMS\n" <<
		"[5] HAS_ANIMATIONS\n" <<
		"Select attributes for this model (space separated):\n";

	std::getline(std::cin, attributes);

	std::istringstream iss(attributes);
	std::string token;

	uint32_t flags = 0;

	while (iss >> token) {
		try {
			int choice = std::stoi(token);
			switch (choice) {
			case 1: flags |= HAS_NORMALS;    break;
			case 2: flags |= HAS_COLORS;     break;
			case 3: flags |= HAS_TEXCOORDS;  break;
			case 4: flags |= HAS_TRANSFORMS; break;
			case 5: flags |= HAS_ANIMATIONS; break;
			default:
				std::cout << "Unknown option: " << choice << "\n";
			}
		}
		catch (const std::invalid_argument&) {
			std::cout << "Invalid input (not a number): " << token << "\n";
			return 0;
		}
	}

	Model model;

	Importer importer;
	importer.load(modelFilepath, &model, flags);

	std::string data;

	Serializer serializer;
	serializer.serialize(&model, &data);

	Exporter exporter;
	exporter.save(outputFilename, outputFilepath, data);

	std::cout << "Successfully saved file.\n";

	Deserializer deserializer;
	ModelObject modelobj = deserializer.deserialize(outputFilename, outputFilepath);

	return 0;
}