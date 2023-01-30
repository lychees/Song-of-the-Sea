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

#ifndef EP_WINDOW_COURSES_H
#define EP_WINDOW_COURSES_H

// Headers
#include <vector>
#include "../window_selectable.h"

namespace Roguelike{
	const static std::string Courses_Title[16] = {
		"文学",
		"科学",
		"史学",
		"武学",
		"魔法",
		"兵法",
		"礼法",
		"历法",
	};

	const static std::string Courses_Help[16] = {
		"文学应该预见未来。",
		"科学的敌人，不比朋友少。",
		"以史为鉴，可知兴替。",
		"文講八法，武講八勢。",
		"用魔法打败魔法。",
		"知彼知己，百戰不殆",
		"人無禮不立，事無禮不成，國無禮不寧。",
		"脚踏实地，仰望星空。",
	};
};

/**
 * Window_Skill class.
 */
class Window_Courses : public Window_Selectable {

public:

	/**
	 * Constructor.
	 */
	Window_Courses(int ix, int iy, int iwidth, int iheight);

	/**
	 * Sets the actor whose courses are displayed.
	 *
	 * @param actor_id ID of the actor.
	 */
	void SetActor(int actor_id);

	/**
	 * Refreshes the skill list.
	 */
	virtual void Refresh();

	/**
	 * Draws a course together with the level.
	 *
	 * @param index index of skill to draw.
	 */
	void DrawItem(int index);

	/**
	 * Updates the help window.
	 */
	void UpdateHelp() override;
	void Update() override;

	int morning, afternoon, night, stage;

protected:

	int actor_id;
};

#endif
