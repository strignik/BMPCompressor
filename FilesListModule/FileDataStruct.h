#pragma once

#include <string>
#include <filesystem>

namespace filestructs {

struct FileData
{
	FileData(const std::string &name,
	         const std::filesystem::path path,
	         const std::uintmax_t size)
	    : name(name)
	    , path(path)
	    , size(size) {}

	std::string name;
	std::filesystem::path path;
	std::uintmax_t size;
};

}
