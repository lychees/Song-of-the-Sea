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
#include <string>
#include <vector>
#include "scene_crpg_status.h"
#include "../game_party.h"
#include "../game_system.h"
#include "../input.h"
#include "../window_gold.h"

Scene_Crpg_Status::Scene_Crpg_Status(int actor_index) :
	actor_index(actor_index) {
	type = Scene::Status;
}

void Scene_Crpg_Status::Start() {
	int actor = Main_Data::game_party->GetActors()[actor_index]->GetId();
	actorinfo_window.reset(new Window_ActorInfo(0, 0, 124, 208, actor));
	crpg_status_window.reset(new Window_Crpg_Status(124, 0, 196, 45, actor));
	gold_window.reset(new Window_Gold(0, 208, 124, 32));
	window_alignment.reset(new Window_Alignment(124, 45, 196, 195, actor));
}

void Scene_Crpg_Status::Update() {

	gold_window->Update();

	if (Input::IsTriggered(Input::CANCEL)) {
		Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Cancel));
		Scene::Pop();
	} else if (Main_Data::game_party->GetActors().size() > 1 && Input::IsTriggered(Input::RIGHT)) {
		Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Cursor));
		actor_index = (actor_index + 1) % Main_Data::game_party->GetActors().size();
		Scene::Push(std::make_shared<Scene_Crpg_Status>(actor_index), true);
	} else if (Main_Data::game_party->GetActors().size() > 1 && Input::IsTriggered(Input::LEFT)) {
		actor_index = (actor_index + Main_Data::game_party->GetActors().size() - 1) % Main_Data::game_party->GetActors().size();
		Scene::Push(std::make_shared<Scene_Crpg_Status>(actor_index), true);
	}
}
