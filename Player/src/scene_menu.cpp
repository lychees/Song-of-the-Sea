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
#include <cassert>
#include "scene_menu.h"
#include "audio.h"
#include "cache.h"
#include "game_party.h"
#include "game_system.h"
#include "input.h"
#include "player.h"
#include "scene_debug.h"
#include "scene_end.h"
#include "scene_equip.h"
#include "scene_item.h"
#include "scene_skill.h"
#include "scene_order.h"
#include "scene_save.h"
#include "scene_status.h"
#include "bitmap.h"
#include "game_multiplayer_settings_scene.h"
#include "uminouta/scene_crpg_status.h"
#include "uminouta/scene_quirks.h"

Scene_Menu::Scene_Menu(int menu_index) :
	menu_index(menu_index) {
	type = Scene::Menu;
}

void Scene_Menu::Start() {
	CreateCommandWindow();

	// Gold Window
	gold_window.reset(new Window_Gold(0, (SCREEN_TARGET_HEIGHT-32), 88, 32));

	// Status Window
	menustatus_window.reset(new Window_MenuStatus(88, 0, (SCREEN_TARGET_WIDTH-88), SCREEN_TARGET_HEIGHT));
	menustatus_window->SetActive(false);
}

void Scene_Menu::Continue(SceneType /* prev_scene */) {
	menustatus_window->Refresh();
	gold_window->Refresh();
}

void Scene_Menu::Update() {
	command_window->Update();
	gold_window->Update();
	menustatus_window->Update();

	if (command_window->GetActive()) {
		UpdateCommand();
	}
	else if (menustatus_window->GetActive()) {
		UpdateActorSelection();
	}
}

void Scene_Menu::CreateCommandWindow() {
	// Create Options Window
	std::vector<std::string> options;

	if (Player::IsRPG2k()) {
		command_options.push_back(Item);
		command_options.push_back(Skill);
		command_options.push_back(Equipment);
		command_options.push_back(Save);
		//if (Player::debug_flag) {
			command_options.push_back(Debug);
		//}
		command_options.push_back(Multiplayer);
		command_options.push_back(Quit);
		if (Player::game_title == "Untitled" ||
			Player::game_title == "海之歌" ||
			Player::game_title == "Song of the Sea" ||
			Player::game_title == "海のうた") {
			command_options.push_back(CRPG_Status);
		}
	} else {
		for (std::vector<int16_t>::iterator it = lcf::Data::system.menu_commands.begin();
			it != lcf::Data::system.menu_commands.end(); ++it) {
				command_options.push_back((CommandOptionType)*it);
		}
		//if (Player::debug_flag) {
			command_options.push_back(Debug);
		//}
		command_options.push_back(Multiplayer);
		command_options.push_back(Quit);
		if (Player::game_title == "Untitled" ||
			Player::game_title == "海之歌" ||
			Player::game_title == "Song of the Sea" ||
			Player::game_title == "海のうた") {
			command_options.push_back(CRPG_Status);
		}
	}

	// Add all menu items
	std::vector<CommandOptionType>::iterator it;
	for (it = command_options.begin(); it != command_options.end(); ++it) {
		switch(*it) {
		case Item:
			options.push_back(ToString(lcf::Data::terms.command_item));
			break;
		case Skill:
			options.push_back(ToString(lcf::Data::terms.command_skill));
			break;
		case Equipment:
			options.push_back(ToString(lcf::Data::terms.menu_equipment));
			break;
		case Save:
			options.push_back(ToString(lcf::Data::terms.menu_save));
			break;
		case Status:
			options.push_back(ToString(lcf::Data::terms.status));
			break;
		case Row:
			options.push_back(ToString(lcf::Data::terms.row));
			break;
		case Order:
			options.push_back(ToString(lcf::Data::terms.order));
			break;
		case Wait:
			options.push_back(ToString(Main_Data::game_system->GetAtbMode() == lcf::rpg::SaveSystem::AtbMode_atb_wait ? lcf::Data::terms.wait_on : lcf::Data::terms.wait_off));
			break;
		case Debug:
			options.push_back("Debug");
			break;
		case Multiplayer:
			options.push_back(Player::IsCP936() ? "通信设置" : (Player::IsBig5() ? "通信設置" : (Player::IsCP932() ? "通信設定" : "Multiplayer")));
			break;
		case CRPG_Status:
//			options.push_back(Player::IsCP936() ? "属性" : (Player::IsBig5() ? "属性" : (Player::IsCP932() ? "属性" : "属性")));
			options.push_back(Player::IsCP936() ? "特性" : (Player::IsBig5() ? "特性" : (Player::IsCP932() ? "特性" : "特性")));
			break;
		default:
			options.push_back(ToString(lcf::Data::terms.menu_quit));
			break;
		}
	}

	command_window.reset(new Window_Command(options, 88));
	command_window->SetIndex(menu_index);

	// Disable items
	for (it = command_options.begin(); it != command_options.end(); ++it) {
		switch(*it) {
		case Save:
			// If save is forbidden disable this item
			if (!Main_Data::game_system->GetAllowSave()) {
				command_window->DisableItem(it - command_options.begin());
			}
		case Wait:
		case Quit:
		case Debug:
		case Multiplayer:
			break;
		case Order:
			if (Main_Data::game_party->GetActors().size() <= 1) {
				command_window->DisableItem(it - command_options.begin());
			}
			break;
		default:
			if (Main_Data::game_party->GetActors().empty()) {
				command_window->DisableItem(it - command_options.begin());
			}
			break;
		}
	}
}

void Scene_Menu::UpdateCommand() {
	if (Input::IsTriggered(Input::CANCEL)) {
		Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Cancel));
		Scene::Pop();
	} else if (Input::IsTriggered(Input::DECISION)) {
		menu_index = command_window->GetIndex();

		switch (command_options[menu_index]) {
		case Item:
			if (Main_Data::game_party->GetActors().empty()) {
				Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Buzzer));
			} else {
				Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Decision));
				Scene::Push(std::make_shared<Scene_Item>());
			}
			break;
		case Skill:
		case Equipment:
		case Status:
		case Row:
		case CRPG_Status:
			if (Main_Data::game_party->GetActors().empty()) {
				Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Buzzer));
			} else {
				Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Decision));
				command_window->SetActive(false);
				menustatus_window->SetActive(true);
				menustatus_window->SetIndex(0);
			}
			break;
		case Save:
			if (!Main_Data::game_system->GetAllowSave()) {
				Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Buzzer));
			} else {
				Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Decision));
				Scene::Push(std::make_shared<Scene_Save>());
			}
			break;
		case Order:
			if (Main_Data::game_party->GetActors().size() <= 1) {
				Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Buzzer));
			} else {
				Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Decision));
				Scene::Push(std::make_shared<Scene_Order>());
			}
			break;
		case Wait:
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Decision));
			Main_Data::game_system->ToggleAtbMode();
			command_window->SetItemText(menu_index,
				Main_Data::game_system->GetAtbMode() == lcf::rpg::SaveSystem::AtbMode_atb_wait ? lcf::Data::terms.wait_on : lcf::Data::terms.wait_off);
			break;
		case Debug:
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Decision));
			Scene::Push(std::make_shared<Scene_Debug>());
			break;
		case Quit:
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Decision));
			Scene::Push(std::make_shared<Scene_End>());
			break;
		case Multiplayer:
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Decision));
			Scene::Push(std::make_shared<Game_Multiplayer::Scene_MultiplayerSettings>());
			break;
		}
	}
}

void Scene_Menu::UpdateActorSelection() {
	if (Input::IsTriggered(Input::CANCEL)) {
		Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Cancel));
		command_window->SetActive(true);
		menustatus_window->SetActive(false);
		menustatus_window->SetIndex(-1);
	} else if (Input::IsTriggered(Input::DECISION)) {
		switch (command_options[command_window->GetIndex()]) {
		case Skill:
			if (!menustatus_window->GetActor()->CanAct()) {
				Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Buzzer));
				return;
			}
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Decision));
			Scene::Push(std::make_shared<Scene_Skill>(menustatus_window->GetIndex()));
			break;
		case Equipment:
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Decision));
			Scene::Push(std::make_shared<Scene_Equip>(*menustatus_window->GetActor()));
			break;
		case Status:
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Decision));
			Scene::Push(std::make_shared<Scene_Status>(menustatus_window->GetIndex()));
			break;
		case CRPG_Status:
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Decision));
			Scene::Push(std::make_shared<Scene_Quirks>(menustatus_window->GetIndex()));
			break;
		case Row:
		{
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Decision));
			// Don't allow entire party in the back row.
			const auto& actors = Main_Data::game_party->GetActors();
			int num_in_back = 0;
			for (auto* actor: actors) {
				if (actor->GetBattleRow() == Game_Actor::RowType::RowType_back) {
					++num_in_back;
				}
			}
			Game_Actor* actor = actors[menustatus_window->GetIndex()];
			if (actor->GetBattleRow() == Game_Actor::RowType::RowType_front) {
				if (num_in_back < int(actors.size() - 1)) {
					actor->SetBattleRow(Game_Actor::RowType::RowType_back);
				}
			} else {
				actor->SetBattleRow(Game_Actor::RowType::RowType_front);
			}
			menustatus_window->Refresh();
			break;
		}
		default:
			assert(false);
			break;
		}

		command_window->SetActive(true);
		menustatus_window->SetActive(false);
		menustatus_window->SetIndex(-1);
	}
}
