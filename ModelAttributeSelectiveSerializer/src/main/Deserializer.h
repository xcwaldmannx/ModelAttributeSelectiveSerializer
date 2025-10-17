#pragma once

#include <string>

#include <nlohmann/json.hpp>

class Deserializer
{
public:
	void deserialize(const std::string& filename, const std::string& filepath, nlohmann::json* json);
};