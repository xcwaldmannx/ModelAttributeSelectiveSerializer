#pragma once

#include "Configuration.h"

#include <string>

namespace mass
{

	class ModelAttributeSelectiveSerializer
	{
	public:
		void serialize(
			const Configuration& config,
			const std::string& inputFilepath,
			const std::string& outputFilepath);
	};

}