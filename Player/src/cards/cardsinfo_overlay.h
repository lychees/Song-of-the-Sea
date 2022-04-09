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

#ifndef EP_CARDSINFO_OVERLAY_H
#define EP_CARDSINFO_OVERLAY_H

#include <deque>
#include <string>
#include <vector>
#include "../drawable.h"
#include "../memory_management.h"
#include "../rect.h"
#include "../game_clock.h"
#include "../sprite.h"
#include "../string_view.h"

struct FloatText {
	std::shared_ptr<Sprite> sprite;
	int remaining_time = 30;
};

/**
 * FpsOverlay class.
 * Shows current FPS and the speedup indicator.
 */
class CardsInfoOverlay : public Drawable {
public:
	CardsInfoOverlay();

	void Draw(Bitmap& dst) override;

	/**
	 * Update the fps overlay.
	 *
	 * @return true if the fps string was changed
	 */
	bool Update();

	/**
	 * Formats a string containing FPS.
	 *
	 * @return fps string
	 */
	std::string GetFpsString() const;

	/**
	 * Set whether we will render the Cardsinfo.
	 *
	 * @param value true if we want to draw to screen
	 */
	void SetDrawCardsinfo(bool value);

	void DrawFloatText(int x, int y, int color, StringView text);

	std::vector<FloatText> floating_texts;

private:
	void UpdateText();

	BitmapRef fps_bitmap;

	/** Rect to draw on screen */
	Rect fps_rect;
	Rect speedup_rect;

	std::string text;
	std::string text2;

	int last_speed_mod = 1;
	bool speedup_dirty = true;
	bool fps_dirty = true;
	bool draw_fps = false;
	bool draw_cardsinfo = false;
};

inline std::string CardsInfoOverlay::GetFpsString() const {
	return text;
}

inline void CardsInfoOverlay::SetDrawCardsinfo(bool value) {
	draw_cardsinfo = value;
}

#endif
