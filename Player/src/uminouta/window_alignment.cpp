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
#include "roguelike.h"
#include "../game_actors.h"
#include "window_alignment.h"
#include "../bitmap.h"
#include "../font.h"

Window_Alignment::Window_Alignment(int ix, int iy, int iwidth, int iheight, int actor_id) :
	Window_Selectable(ix, iy, iwidth, iheight),
	actor_id(actor_id)
{

	SetContents(Bitmap::Create(width - 16, height - 16));

	Refresh();
}

void Window_Alignment::Refresh() {

	CreateContents();
	SetIndex(index);

	contents->Clear();

	auto* actor = Main_Data::game_actors->GetActor(actor_id);

	auto draw = [this](int x, int y, StringView name) {
		// Draw Term
		contents->TextDraw(x+0, y, Font::ColorDefault, name);
		/*
		// Draw Value
		contents->TextDraw(x+45, y, Font::ColorDefault, std::to_string(value), Text::AlignRight);
		*/
		return;
	};


	auto player = Roguelike::get_Player();
	int x = 0, y = 2;
	draw(x, y, player.alignments());
	y += 16;

	for (auto b: player.buffs) {
		draw(x, y, b);
		if (x == 0) x += 90;
		else x = 0, y += 16;
	}
}
