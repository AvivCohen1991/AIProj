#pragma once

#include <string>
#include <filesystem>

namespace fs = std::experimental::filesystem;

class ReplayResources {
public:
	static const std::string DIR_PATH;
	static const std::string LAST_REPLAY_PATH;
	static const std::string REPLAY_PATH_PREFIX_WITHOUT_INDEX;
};

class RecordsResources {
public:
	static const std::string FILE_NAME;
};