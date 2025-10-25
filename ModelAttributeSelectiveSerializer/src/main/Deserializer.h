#pragma once

#include "ModelObject.h"

#include <string>


class Deserializer
{
public:
	ModelObject deserialize(const std::string& filename, const std::string& filepath);
};