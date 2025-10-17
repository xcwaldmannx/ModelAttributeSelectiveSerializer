#pragma once

#include "Importer.h"

#include <string>


class Serializer
{
public:
	void serialize(Model* model, std::string* data);
};