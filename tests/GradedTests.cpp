#include "catch.hpp"
#include "SrcMain.h"
#include "HashTable.hpp"
#include <string>
#include <chrono>
#include <algorithm>

// Helper function declarations (don't change these)
extern bool CheckFileMD5(const std::string& fileName, const std::string& expected);
extern bool CheckTextFilesSame(const std::string& fileNameA,
	const std::string& fileNameB);
