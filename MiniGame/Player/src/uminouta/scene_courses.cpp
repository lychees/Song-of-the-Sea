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
#include "scene_courses.h"
#include "../input.h"
#include "../output.h"
#include "../transition.h"
#include "../game_system.h"

Scene_Courses::Scene_Courses(int index) :
	index(index) {
	Scene::type = Scene::Item;
}

void Scene_Courses::Start() {
	// Create the windows
	help_window.reset(new Window_Help(0, 0, SCREEN_TARGET_WIDTH, 32));
	courses_window.reset(new Window_Courses(0, 32, SCREEN_TARGET_WIDTH, SCREEN_TARGET_HEIGHT - 32));

	courses_window->SetIndex(index);
	courses_window->SetHelpWindow(help_window.get());
	courses_window->Refresh();
}

void Scene_Courses::Continue(SceneType /* prev_scene */) {
	courses_window->Refresh();
}

void Scene_Courses::Update() {
	help_window->Update();
	courses_window->Update();

	if (Input::IsTriggered(Input::CANCEL)) {
		Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Cancel));
		Scene::Pop();
	} else if (Input::IsTriggered(Input::DECISION)) {
		//int item_id = quirks_window->GetItem() == NULL ? 0 : item_window->GetItem()->ID;
		Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Decision));
//		Scene::Push(std::make_shared<Scene_ActorTarget>(item_id));
		index = courses_window->GetIndex();
	}
}

void Scene_Courses::TransitionOut(Scene::SceneType next_scene) {
	Scene::TransitionOut(next_scene);
}
