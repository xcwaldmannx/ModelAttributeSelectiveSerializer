#pragma once

#include "Configuration.h"

#include <string>

namespace mal
{

	void serialize(
		const Configuration& config,
		const std::string& inputFilepath,
		const std::string& outputFilepath);

	ModelLayout deserialize(
		const Configuration& config,
		const std::string& inputFilepath);

}