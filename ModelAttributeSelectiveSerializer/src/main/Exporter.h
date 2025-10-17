#pragma once

#include <string>

class Exporter
{
public:
	void save(const std::string& filename, const std::string& filepath, const std::string& data);
};