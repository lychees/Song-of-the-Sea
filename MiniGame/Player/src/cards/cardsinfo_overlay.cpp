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

#include <algorithm>
#include <sstream>

#include "cardsinfo_overlay.h"
#include "../game_clock.h"
#include "../bitmap.h"
#include "../utils.h"
#include "../input.h"
#include "../font.h"
#include "../drawable_mgr.h"
#include "../output.h"
#include "../scene_map.h"
#include "../sprite_character.h"
#include "../cache.h"
#include "cards.h"


using namespace std::chrono_literals;

static constexpr auto refresh_frequency = 1s;

CardsInfoOverlay::CardsInfoOverlay() :
	Drawable(Priority_Overlay + 100, Drawable::Flags::Global)
{
	DrawableMgr::Register(this);

	UpdateText();
}

void CardsInfoOverlay::DrawFloatText(int x, int y, int color, StringView text) {
	Rect rect = Font::Default()->GetSize(text);

	BitmapRef graphic = Bitmap::Create(rect.width, rect.height);
	graphic->Clear();
	graphic->TextDraw(-rect.x, -rect.y, color, text);

	std::shared_ptr<Sprite> floating_text = std::make_shared<Sprite>();
	floating_text->SetBitmap(graphic);
	floating_text->SetOx(rect.width / 2);
	floating_text->SetOy(rect.height + 5);
	floating_text->SetX(x);
	// Move 5 pixel down because the number "jumps" with the intended y as the peak
	floating_text->SetY(y + 5);
	floating_text->SetZ(Priority_Window + y);

	FloatText float_text;
	float_text.sprite = floating_text;

	floating_texts.push_back(float_text);
}

void CardsInfoOverlay::UpdateText() {
	// auto fps = Utils::RoundTo<int>(Game_Clock::GetFPS());

	Cards::Instance& _ = Cards::instance();
	text = "HP: " + std::to_string(_.hp)  + " " +
		   "MP: " + std::to_string(_.mp) + "/" + std::to_string(_.MP) + " " +
		   "手牌: " + std::to_string(_.hand.size()) + " " +
		   "卡组: " + std::to_string(_.deck.size());

	text2  = "HP: " + std::to_string(_.ai_hp)  + " " +
		   "MP: " + std::to_string(_.ai_mp) + "/" + std::to_string(_.ai_MP) + " " +
		   "手牌: " + std::to_string(_.ai_hand.size()) + " " +
		   "卡组: " + std::to_string(_.ai_deck.size());

	fps_dirty = true;
}

bool CardsInfoOverlay::Update() {

	for (auto it = floating_texts.begin(); it != floating_texts.end();) {
		int &time = it->remaining_time;

		if (time % 2 == 0) {
			int modifier = time <= 10 ? 1 :
						   time < 20 ? 0 :
						   -1;
			it->sprite->SetY(it->sprite->GetY() + modifier);
		}

		--time;
		if (time <= 0) {
			it = floating_texts.erase(it);
		} else {
			++it;
		}
	}

	UpdateText();
	return true;
}

void CardsInfoOverlay::Draw(Bitmap& dst) {

	if (!draw_cardsinfo) return;

	Cards::Instance& _ = Cards::instance();

	if (_.pause) return;
	Scene_Map* scene = (Scene_Map*)Scene::Find(Scene::Map).get();
	std::string t; Rect rect;

	int x = 20, y = 180;
	for (auto &m: _.hand) {
		t = m.name.substr(0, std::min(6, int(m.name.size()))) + std::to_string(m.cost);
		rect = Font::Default()->GetSize(t);
		auto bitmap = Bitmap::Create(rect.width + 1, rect.height - 1, true);
		bitmap->Clear();
		bitmap->Fill(Color(0, 0, 0, 128));
		bitmap->TextDraw(1, 0, Color(255, 255, 255, 255), t);
		dst.Blit(x, y, *bitmap, rect, 255);

		t = std::to_string(m.AP);
		rect = Font::Default()->GetSize(t);
		bitmap = Bitmap::Create(rect.width + 1, rect.height - 1, true);
		bitmap->Clear();
		bitmap->Fill(Color(0, 0, 0, 128));
		bitmap->TextDraw(1, 0, Color(255, 255, 255, 255), t);
		dst.Blit(x + 24, y + 10, *bitmap, rect, 255);

		t = std::to_string(m.HP);
		rect = Font::Default()->GetSize(t);
		bitmap = Bitmap::Create(rect.width + 1, rect.height - 1, true);
		bitmap->Clear();
		bitmap->Fill(Color(0, 0, 0, 128));
		bitmap->TextDraw(1, 0, Color(255, 255, 255, 255), t);
		dst.Blit(x + 24, y + 20, *bitmap, rect, 255);

		auto character_name = _.json[m.key]["charset"];
		int offset = _.json[m.key]["offset"];
		bitmap = Cache::Charset(character_name);
		rect = Sprite_Character::GetCharacterRect(character_name, offset, rect);
		rect.x += 24;
		rect.y += 64;
		rect.width = 24; rect.height = 32;
		dst.Blit(x, y, *bitmap, rect, 255);
		x += 45;
	}

	for (auto &m: _.battlefield) {

		t = std::to_string(m.AP);
		rect = Font::Default()->GetSize(t);
		// rect.width /= 2; rect.height /= 2;

		auto s = scene->spriteset->FindCharacter(Game_Map::GetEvent(m.id));

		auto bitmap = Bitmap::Create(rect.width + 1, rect.height - 1, true);
		//new Bitmap(rect.width, rect.height, 1);
		bitmap->Clear();
		bitmap->Fill(Color(0, 0, 0, 128));
		bitmap->TextDraw(1, 0, m.AP < (_.json[m.key])["AP"] ? Color(255, 140, 140, 255) : m.AP > (_.json[m.key])["AP"] ? Color(140, 255, 140, 255) : Color(255, 255, 255, 255), t);
		dst.Blit(s->GetX()-6, s->GetY() - 26, *bitmap, rect, 255);

		t = std::to_string(m.hp);
		rect = Font::Default()->GetSize(t);
		auto bitmap2 = Bitmap::Create(rect.width + 1, rect.height - 1, true);
		bitmap2->Clear();
		bitmap2->Fill(Color(0, 0, 0, 128));
		bitmap2->TextDraw(1, 0, m.hp < (_.json[m.key])["HP"] ? Color(255, 140, 140, 255) : m.hp > (_.json[m.key])["HP"] ? Color(140, 255, 140, 255) : Color(255, 255, 255, 255), t);
		dst.Blit(s->GetX()-6, s->GetY() - 10, *bitmap2, rect, 255);
	}

	if (true || fps_dirty) {
		Rect rect = Font::Default()->GetSize(text);

		if (!fps_bitmap || fps_bitmap->GetWidth() < rect.width + 1) {
			// Height never changes
			fps_bitmap = Bitmap::Create(rect.width + 1, rect.height - 1, true);
		}
		fps_bitmap->Clear();
		fps_bitmap->Fill(Color(0, 0, 0, 128));
		fps_bitmap->TextDraw(1, 0, Color(255, 255, 255, 255), text);
		fps_rect = Rect(0, 0, rect.width + 1, rect.height - 1);

		fps_dirty = false;
		dst.Blit(300 - rect.width, 220, *fps_bitmap, fps_rect, 255);
	}

	if (true || fps_dirty) {
		Rect rect = Font::Default()->GetSize(text2);

		if (!fps_bitmap || fps_bitmap->GetWidth() < rect.width + 1) {
			// Height never changes
			fps_bitmap = Bitmap::Create(rect.width + 1, rect.height - 1, true);
		}
		fps_bitmap->Clear();
		fps_bitmap->Fill(Color(0, 0, 0, 128));
		fps_bitmap->TextDraw(1, 0, Color(255, 255, 255, 255), text2);
		fps_rect = Rect(0, 0, rect.width + 1, rect.height - 1);

		fps_dirty = false;
		dst.Blit(20, 10, *fps_bitmap, fps_rect, 255);
	}
}

