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
#include "scene_cards.h"
#include "../game_map.h"
#include "../game_party.h"
#include "../game_player.h"
#include "../game_switches.h"
#include "../game_system.h"
#include "../game_targets.h"
#include "../input.h"
#include <lcf/reader_util.h>
#include "../scene_actortarget.h"
#include "../scene_map.h"
#include "../scene_teleport.h"
#include "../output.h"
#include "../transition.h"

Scene_Cards::Scene_Cards(int cards_position, int item_index) :
	cards_position(cards_position), item_index(item_index) {
	Scene::type = Scene::Item;
}

void Scene_Cards::Start() {
	// Create the windows
	help_window.reset(new Window_Help(0, 0, SCREEN_TARGET_WIDTH, 32));
	item_window.reset(new Window_Cards(cards_position, 0, 32, SCREEN_TARGET_WIDTH, SCREEN_TARGET_HEIGHT - 32));
	item_window->SetHelpWindow(help_window.get());
	item_window->Refresh();
	item_window->SetIndex(item_index);
}

void Scene_Cards::Continue(SceneType /* prev_scene */) {
	item_window->Refresh();
}

void Scene_Cards::Update() {
	help_window->Update();
	item_window->Update();
	item_index = item_window->GetIndex();

	if (Input::IsTriggered(Input::CANCEL)) {
		Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Cancel));
		Scene::Pop();
	} else if (Input::IsTriggered(Input::DECISION)) {
		if (item_window->CheckEnable(item_index)) {
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Decision));
			Cards::instance().selected_id = item_index;
			Scene::PopUntil(Scene::Map);
		} else {
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Buzzer));
		}
	}
}

void Scene_Cards::TransitionOut(Scene::SceneType next_scene) {

	/*const auto* item = item_window->GetItem();
	const lcf::rpg::Skill* skill = nullptr;
	if (item && item->type == lcf::rpg::Item::Type_special && item->skill_id > 0) {
		skill = lcf::ReaderUtil::GetElement(lcf::Data::skills, item->skill_id);
	}
	if (next_scene == Map && skill && skill->type == lcf::rpg::Skill::Type_escape) {
		Transition::instance().InitErase(Transition::TransitionFadeOut, this);
	} else {
		Scene::TransitionOut(next_scene);
	}*/
	Scene::TransitionOut(next_scene);
	// Transition::instance().InitErase(Transition::TransitionFadeOut, this);
}
