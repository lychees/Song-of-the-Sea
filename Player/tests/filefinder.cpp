#include <cassert>
#include <cstdlib>
#include "filefinder.h"
#include "player.h"
#include "main_data.h"
#include "doctest.h"

TEST_SUITE_BEGIN("FileFinder");

TEST_CASE("IsRPG2kProject") {
	Main_Data::Init();

	Player::escape_symbol = "\\";

	auto fs = FileFinder::Root().Subtree(EP_TEST_PATH "/game");
	CHECK(FileFinder::IsRPG2kProject(fs));

	Player::escape_symbol = "";
}

TEST_CASE("IsNotRPG2kProject") {
	Main_Data::Init();

	auto fs = FileFinder::Root().Subtree(EP_TEST_PATH "/notagame");
	CHECK(!FileFinder::IsRPG2kProject(fs));
}

TEST_CASE("MakeCanonical") {
	auto make_canonical = [](const char* s, int deep) {
		auto canon = FileFinder::MakeCanonical(s, deep);
		#ifdef _WIN32
		std::replace(canon.begin(), canon.end(), '\\', '/');
		#endif
		return canon;
	};

	CHECK(make_canonical("/folder/file", 1) == "/folder/file");
	CHECK(make_canonical("folder/file", 1) == "folder/file");
	CHECK(make_canonical("file", 1) == "file");
	CHECK(make_canonical("/path/../path2", 1) == "/path2");
	CHECK(make_canonical("folder/././//file", 0) == "folder/file");
	CHECK(make_canonical("folder/././//file", 0) == "folder/file");
	CHECK(make_canonical("folder/../file", 1) == "file");
	CHECK(make_canonical("../folder/folder2/../file", 1) == "folder/file");
	CHECK(make_canonical("folder/../../file", 1) == "file");

	Player::escape_symbol = "\\";
	CHECK(make_canonical("..\\folder\\folder2\\..\\file", 1) == "folder/file");
	CHECK(make_canonical("folder\\..\\..\\file", 1) == "file");
	Player::escape_symbol = "";
}

TEST_CASE("GetPathAndFilename") {
	// This function converts all paths to "/" to simplify the DirectoryTree
	// No non-canonical paths tested, the path is expected to be canonical

	std::string path, file;

	std::tie(path, file) = FileFinder::GetPathAndFilename("folder/file");
	CHECK(path == "folder");
	CHECK(file == "file");

	std::tie(path, file) = FileFinder::GetPathAndFilename("/file");
	CHECK(path == "");
	CHECK(file == "file");

	std::tie(path, file) = FileFinder::GetPathAndFilename("file");
	CHECK(path == "");
	CHECK(file == "file");

	std::tie(path, file) = FileFinder::GetPathAndFilename("folder/folder2/file");
	CHECK(path == "folder/folder2");
	CHECK(file == "file");

	Player::escape_symbol = "\\";
	std::tie(path, file) = FileFinder::GetPathAndFilename("folder\\folder2\\file");
	CHECK(path == "folder/folder2");
	CHECK(file == "file");
	Player::escape_symbol = "";
}

TEST_SUITE_END();
