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

// Headers
#include <cstdlib>
#include "main_data.h"
#include "filefinder.h"
#include "filefinder_rtp.h"
#include "filesystem.h"
#include "game_system.h"
#include "game_actors.h"
#include "game_party.h"
#include "game_enemyparty.h"
#include "game_ineluki.h"
#include "game_player.h"
#include "game_screen.h"
#include "game_pictures.h"
#include "game_map.h"
#include "game_variables.h"
#include "game_switches.h"
#include "game_targets.h"
#include "game_quit.h"
#include "font.h"
#include "player.h"
#include "system.h"
#include "output.h"

#ifndef _WIN32
#  include <unistd.h>
#endif
#if defined(USE_SDL) && defined(__ANDROID__)
#  include <jni.h>
#  include <SDL_system.h>
#elif defined(_3DS)
#  include <3ds.h>
#  include <cstdio>
#elif defined(PSP2)
#  include <cstdio>
#  include <psp2/io/stat.h>
#elif defined(__APPLE__) && TARGET_OS_OSX
#  include <sys/syslimits.h>
#  include "platform/macos/utils.h"
#endif

// Global variables.
std::string project_path;

namespace Main_Data {
	// Dynamic Game lcf::Data
	std::unique_ptr<Game_System> game_system;
	std::unique_ptr<Game_Switches> game_switches;
	std::unique_ptr<Game_Variables> game_variables;
	std::unique_ptr<Game_Screen> game_screen;
	std::unique_ptr<Game_Pictures> game_pictures;
	std::unique_ptr<Game_Actors> game_actors;
	std::unique_ptr<Game_Player> game_player;
	std::unique_ptr<Game_Party> game_party;
	std::unique_ptr<Game_EnemyParty> game_enemyparty;
	std::unique_ptr<Game_Targets> game_targets;
	std::unique_ptr<Game_Quit> game_quit;
	std::unique_ptr<Game_Ineluki> game_ineluki;
	std::unique_ptr<FileFinder_RTP> filefinder_rtp;
}

void Main_Data::Init() {
	if (project_path.empty()) {
		project_path =
			getenv("RPG_TEST_GAME_PATH") ? getenv("RPG_TEST_GAME_PATH") :
			getenv("RPG_GAME_PATH") ? getenv("RPG_GAME_PATH") :
			"";

		if (project_path.empty()) {
			// first set to current directory for all platforms
			project_path = "";

#if defined(GEKKO) || defined(__SWITCH__) || defined(__MORPHOS__) || defined(__amigaos4__) || defined(__AROS__)
			// Working directory not correctly handled
			char working_dir[256];
			getcwd(working_dir, 255);
			project_path = std::string(working_dir);
#elif defined(PSP2)
			// Check if app0 filesystem contains the title id reference file
			FILE* f = fopen("app0:/titleid.txt","r");
			if (f == NULL)
				project_path = "ux0:/data/easyrpg-player";
			else {
				char titleID[10];
				char psp2_dir[256];
				fread(titleID, 1, 9, f);
				titleID[9] = 0;
				sprintf(psp2_dir, "ux0:/data/%s",titleID);
				fclose(f);
				project_path = "app0:";
				// Creating saves dir if it doesn't exist
				sceIoMkdir(psp2_dir, 0777);
				auto savefs = FileFinder::Root().Create(psp2_dir);
				if (!savefs) {
					Output::Error("PSP2: Invalid save path {}", psp2_dir);
				}
				FileFinder::SetSaveFilesystem(savefs);
			}
#elif defined(_3DS)
			// Check if romFs has some files inside or not
			FILE* testfile = fopen("romfs:/RPG_RT.lmt","r");
			if (testfile != NULL) {
				Output::Debug("Detected a project on romFs filesystem...");
				fclose(testfile);
				project_path = "romfs:";

				if (!Player::is_3dsx) {
					// Create savepath for CIA - unique for any ID

					// Generating save_path
					u64 titleID;
					APT_GetProgramID(&titleID);
					char mainDir[64];
					sprintf(mainDir,"sdmc:/easyrpg-player/%016llX",titleID);

					// Creating dirs if they don't exist
					FS_Archive archive;
					FSUSER_OpenArchive(&archive, ARCHIVE_SDMC, {PATH_EMPTY, 1, (u8*)""});
					FS_Path filePath=fsMakePath(PATH_ASCII, "/easyrpg-player");
					FSUSER_CreateDirectory(archive,filePath, FS_ATTRIBUTE_DIRECTORY);
					FS_Path filePath2=fsMakePath(PATH_ASCII, &mainDir[5]);
					FSUSER_CreateDirectory(archive,filePath2, FS_ATTRIBUTE_DIRECTORY);
					FSUSER_CloseArchive(archive);

					auto savefs = FileFinder::Root().Create(mainDir);
					if (!savefs) {
						Output::Error("3DS: Invalid save path {}", mainDir);
					}
					FileFinder::SetSaveFilesystem(savefs);
				}
			} else if (!Player::is_3dsx) {
				// No RomFS -> load games from hardcoded path
				project_path = "sdmc:/3ds/easyrpg-player";
			}
#elif defined(__APPLE__) && TARGET_OS_OSX
			// Apple Finder does not set the working directory
			// It points to HOME instead. When it is HOME change it to
			// the application directory instead

			char* home = getenv("HOME");
			char current_dir[PATH_MAX] = { 0 };
			getcwd(current_dir, sizeof(current_dir));
			if (strcmp(current_dir, "/") == 0 || strcmp(current_dir, home) == 0) {
				project_path = MacOSUtils::GetBundleDir();
			}
#endif
		}
	}
}


void Main_Data::Cleanup() {
	Game_Map::Quit();

	game_switches.reset();
	game_screen.reset();
	game_pictures.reset();
	game_player.reset();
	game_party.reset();
	game_enemyparty.reset();
	game_actors.reset();
	game_targets.reset();
	game_quit.reset();
	game_system.reset();
	game_ineluki.reset();
}

const std::string& Main_Data::GetDefaultProjectPath() {
	return project_path;
}
