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
#include <iomanip>
#include <sstream>
#include <lcf/reader_util.h>
#include "window_courses.h"
#include "roguelike.h"
#include "../game_actor.h"
#include "../game_actors.h"
#include "../game_party.h"
#include "../bitmap.h"
#include "../font.h"
#include "../player.h"
#include "../output.h"
#include "../game_battle.h"
#include "../output.h"
#include "../input.h"


Window_Courses::Window_Courses(int ix, int iy, int iwidth, int iheight) :
	Window_Selectable(ix, iy, iwidth, iheight), actor_id(0) {
	column_max = 1;
	stage = 0;
}

void Window_Courses::SetActor(int actor_id) {
	this->actor_id = actor_id;
	Refresh();
}

void Window_Courses::Refresh() {

	auto player = Roguelike::get_Player();
	item_max = 8;
	CreateContents();
	contents->Clear();

	for (int i = 0; i < item_max; ++i) {
		DrawItem(i);
	}

	SetIndex(index);
	// SetIndex(1);
}

void Window_Courses::DrawItem(int id) {

	auto& player = Roguelike::get_Player();
	int color = Font::ColorDefault;

	std::string title = Roguelike::Courses_Title[id];
	Rect rect = GetItemRect(id);
	contents->ClearRect(rect);

	// Output::Debug("Update: {} {} {} {} {}", id, player.morning_course, player.afternoon_course, player.night_course, stage);

	if (player.morning_course == id) title += "[上午]";
	if (player.afternoon_course == id) title += "[下午]";
	if (player.night_course == id) title += "[晚自习]";

	contents->TextDraw(rect.x, rect.y, color, title);
	contents->TextDraw(GetWidth() - 16, rect.y, color, std::to_string(player.Course_Score[id]), Text::AlignRight);
}

void Window_Courses::UpdateHelp() {
	help_window->SetText(Roguelike::Courses_Help[index]);
}

void Window_Courses::Update() {

	Window_Selectable::Update();
	auto& player = Roguelike::get_Player();

	if (Input::IsTriggered(Input::DECISION)) {
		if (stage == 0) {
			player.morning_course = index;
		} else if (stage == 1) {
			player.afternoon_course = index;
		} else if (stage == 2) {
			player.night_course = index;
		}
		++stage; if (stage == 3) stage = 0;
		Refresh();
	}
}
