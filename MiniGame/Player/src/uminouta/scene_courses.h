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

#ifndef EP_SCENE_COURSES_H
#define EP_SCENE_COURSES_H

// Headers
#include "../scene.h"
#include "../window_help.h"

#include "window_courses.h"

/**
 * Scene_Courses courses menu class.
 */
class Scene_Courses : public Scene {
public:
	/**
	 * Constructor.
	 *
	 * @param index index to select.
	 */
	Scene_Courses(int index = 0);

	void Start() override;
	void Continue(SceneType prev_scene) override;
	void Update() override;
	void TransitionOut(Scene::SceneType next_scene) override;

private:
	/** Displays description about the selected item. */
	std::unique_ptr<Window_Help> help_window;
	/** Displays available items. */
	std::unique_ptr<Window_Courses> courses_window;
	/** Index of item selected on startup. */
	int index;
};

#endif
