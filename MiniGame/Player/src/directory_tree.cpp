/*
 * This file is part of EasyRPG Player.
 *
 * EasyRPG Player is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * EasyRPG Player is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with EasyRPG Player. If not, see <http://www.gnu.org/licenses/>.
 */

#include "directory_tree.h"
#include "filefinder.h"
#include "filesystem.h"
#include "output.h"
#include "platform.h"
#include "player.h"
#include "output.h"
#include <lcf/reader_util.h>

#ifdef EP_DEBUG_DIRECTORYTREE
template <typename... Args>
static void DebugLog(const char* fmt, Args&&... args) {
	Output::Debug(fmt, std::forward<Args>(args)...);
}
#else
template <typename... Args>
static void DebugLog(const char*, Args&&...) {}
#endif

namespace {
	std::string make_key(StringView n) {
		return lcf::ReaderUtil::Normalize(n);
	};
}

std::unique_ptr<DirectoryTree> DirectoryTree::Create() {
	return std::make_unique<DirectoryTree>();
}

std::unique_ptr<DirectoryTree> DirectoryTree::Create(Filesystem& fs) {
	std::unique_ptr<DirectoryTree> tree = std::make_unique<DirectoryTree>();
	tree->fs = &fs;

	return tree;
}

DirectoryTree::DirectoryListType* DirectoryTree::ListDirectory(StringView path) const {
	std::vector<Entry> entries;
	std::string fs_path = ToString(path);

	DebugLog("ListDirectory: {}", fs_path);


	auto dir_key = make_key(fs_path);

	auto dir_it = dir_cache.find(dir_key);
	if (dir_it != dir_cache.end()) {
		// Already cached
		DebugLog("ListDirectory Cache Hit: {}", dir_key);
		auto file_it = fs_cache.find(dir_key);
		assert(file_it != fs_cache.end());
		return &file_it->second;
	}

	assert(fs_cache.find(dir_key) == fs_cache.end());

	if (!fs->Exists(fs_path)) {
		std::string parent_dir, child_dir;
		std::tie(parent_dir, child_dir) = FileFinder::GetPathAndFilename(fs_path);

		// Go up and determine the proper casing of the folder
		auto* parent_tree = ListDirectory(parent_dir);
		if (!parent_tree) {
			return nullptr;
		}

		auto parent_key = make_key(parent_dir);
		auto parent_it = dir_cache.find(parent_key);
		assert(parent_it != dir_cache.end());

		auto child_key = make_key(child_dir);
		auto child_it = parent_tree->find(child_key);
		if (child_it != parent_tree->end()) {
			fs_path = FileFinder::MakePath(parent_it->second, child_it->second.name);
		} else {
			return nullptr;
		}
	}

	if (!fs->GetDirectoryContent(fs_path, entries)) {
		return nullptr;
	}

	dir_cache[dir_key] = fs_path;

	DirectoryListType fs_cache_entry;

	for (auto& entry : entries) {
		std::string new_entry_key = make_key(entry.name);

		if (entry.type == FileType::Directory) {
			if (fs_cache_entry.find(new_entry_key) != fs_cache_entry.end()) {
				Output::Warning("The folder \"{}\" exists twice.", entry.name);
				Output::Warning("This can lead to file not found errors. Merge the directories manually in a file browser.");
			}
		}
		fs_cache_entry.emplace(std::make_pair(std::move(new_entry_key), entry));
	}
	fs_cache.emplace(dir_key, fs_cache_entry);

	return &fs_cache.find(dir_key)->second;
}

void DirectoryTree::ClearCache(StringView path) const {
	DebugLog("ClearCache: {}", path);

	if (path.empty()) {
		fs_cache.clear();
		dir_cache.clear();
		return;
	}

	auto dir_key = make_key(path);
	auto fs_it = fs_cache.find(dir_key);
	if (fs_it != fs_cache.end()) {
		fs_cache.erase(fs_it);
	}
	auto dir_it = dir_cache.find(dir_key);
	if (dir_it != dir_cache.end()) {
		dir_cache.erase(dir_it);
	}
}

std::string DirectoryTree::FindFile(StringView filename, Span<StringView> exts) const {
	return FindFile({ ToString(filename), exts });
}

std::string DirectoryTree::FindFile(StringView directory, StringView filename, Span<StringView> exts) const {
	return FindFile({ FileFinder::MakePath(directory, filename), exts });
}

std::string DirectoryTree::FindFile(const DirectoryTree::Args& args) const {
	std::string dir, name, canonical_path;
	// Few games (e.g. Yume2kki) use path traversal (..) in the filenames to point
	// to files outside of the actual directory.
	canonical_path = FileFinder::MakeCanonical(args.path, args.canonical_initial_deepness);

	std::tie(dir, name) = FileFinder::GetPathAndFilename(canonical_path);

	DebugLog("FindFile: {} | {} | {} | {}", args.path, canonical_path, dir, name);

	auto* entries = ListDirectory(dir);
	if (!entries) {
		if (args.file_not_found_warning) {
			Output::Debug("Cannot find: {}/{}", dir, name);
		}
		return "";
	}

	std::string dir_key = make_key(dir);
	auto dir_it = dir_cache.find(dir_key);
	assert(dir_it != dir_cache.end());

	std::string name_key = make_key(name);
	if (args.exts.empty()) {
		auto entry_it = entries->find(name_key);
		if (entry_it != entries->end() && entry_it->second.type == FileType::Regular) {
			return FileFinder::MakePath(dir_it->second, entry_it->second.name);
		}
	} else {
		for (const auto& ext : args.exts) {
			auto full_name_key = name_key + ToString(ext);
			auto entry_it = entries->find(full_name_key);
			if (entry_it != entries->end() && entry_it->second.type == FileType::Regular) {
				return FileFinder::MakePath(dir_it->second, entry_it->second.name);
			}
		}
	}

	if (args.file_not_found_warning) {
		Output::Debug("Cannot find: {}/{}", dir, name);
	}

	return "";
}
