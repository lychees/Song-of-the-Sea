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

#ifndef EP_SCENE_CRPG_STATUS_H
#define EP_SCENE_CRPG_STATUS_H

// Headers
#include "../scene.h"
#include "window_crpg_status.h"
#include "window_alignment.h"
#include "../window_gold.h"
#include "../window_actorinfo.h"

/**
 * Scene Crpg Status class.
 * Displays crpg status information about a party member.
 */
class Scene_Crpg_Status : public Scene {
public:
	/**
	 * Constructor.
	 *
	 * @param actor_index party index of the actor.
	 */
	Scene_Crpg_Status(int actor_index);

	void Start() override;
	void Update() override;

private:
	int actor_index;

	std::unique_ptr<Window_Crpg_Status> crpg_status_window;
	std::unique_ptr<Window_Gold> gold_window;
	std::unique_ptr<Window_ActorInfo> actorinfo_window;
	std::unique_ptr<Window_Alignment> window_alignment;
};

#endif
