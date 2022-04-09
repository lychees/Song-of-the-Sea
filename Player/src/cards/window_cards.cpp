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
#include "../window_item.h"
#include "../game_party.h"
#include "../bitmap.h"
#include "../font.h"
#include "../game_battle.h"
#include "../output.h"
#include "cards.h"
#include "window_cards.h"

Window_Cards::Window_Cards(int cards_position, int ix, int iy, int iwidth, int iheight) :
	cards_position(cards_position),
	Window_Selectable(ix, iy, iwidth, iheight) {
	column_max = 2;
}

bool Window_Cards::CheckInclude(int item_id) {
	if (data->size() == 0 && item_id == 0) {
		return true;
	} else {
		return (item_id > 0);
	}
}

bool Window_Cards::CheckEnable(int item_id) {
	return true;
	// Cards::Instance& _ = Cards::instance();
	// return (*data)[item_id].cost <= _.mp;

	/*auto* item = lcf::ReaderUtil::GetElement(lcf::Data::items, item_id);
	if (!item) {
		return false;
	}
	if (item->type == lcf::rpg::Item::Type_medicine
			&& (!Game_Battle::IsBattleRunning() || !item->occasion_field1)) {
		return true;
	}
	return Main_Data::game_party->IsItemUsable(item_id, actor); */
}

void Window_Cards::Refresh() {
	/*for (size_t i = 0; i < _.hand.size(); ++i) {
		if (this->CheckInclude(_.hand[i].id)) {
			data.push_back(_.hand[i]);
		}
	}*/
	Cards::Instance& _ = Cards::instance();
	if (cards_position == 0) {
		data = &_.deck;
	} else if (cards_position == 1) {
		data = &_.hand;
	} else if (cards_position == 2) {
		data = &_.grave;
	} else {
		data = &_.battlefield;
	}
	item_max = data->size();

	CreateContents();

	SetIndex(index);

	contents->Clear();

	for (int i = 0; i < item_max; ++i) {
		DrawItem(i);
	}
}

void Window_Cards::DrawCardName(std::string name, int cx, int cy, bool enabled) const {
	int color = enabled ? Font::ColorDefault : Font::ColorDisabled;
	contents->TextDraw(cx, cy, color, name);
}

void Window_Cards::DrawItem(int index) {
	Rect rect = GetItemRect(index);
	contents->ClearRect(rect);
	DrawCardName((*data)[index].name, rect.x, rect.y, 1);
}

void Window_Cards::UpdateHelp() {
	Cards::Instance& _ = Cards::instance();
	if (cards_position == 0) {
		data = &_.deck;
	} else if (cards_position == 1) {
		data = &_.hand;
	} else if (cards_position == 2) {
		data = &_.grave;
	} else {
		data = &_.battlefield;
	}
	if (~index) help_window->SetText((*data)[index].info());
}

void Window_Cards::SetActor(Game_Actor * actor) {
	// this->actor = actor;
}
