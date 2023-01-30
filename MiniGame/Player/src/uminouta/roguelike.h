#ifndef ROGUELIKE_H
#define ROGUELIKE_H

#include "libtcod.h"

namespace Roguelike {



	struct Creature {
		int str, dex, con;
		int inT, wis, cha;
		int evil, chaos;
		int Course_Score[8];
		int morning_course;
		int afternoon_course;
		int night_course;
		std::vector<std::string> buffs;
		std::vector<std::pair<std::string, int>> quirks;
		std::string race;
		std::string alignments() {
			std::string z;
			if (chaos > 0) {
				z += "混乱: "; z += std::to_string(chaos);
			} else {
				z += "守序: "; z += std::to_string(-chaos);
			}

			if (evil > 0) {
				z += " 邪恶: "; z += std::to_string(evil);
			} else {
				z += " 善良: "; z += std::to_string(-evil);
			}

			z += " 阵营: 守序善良";
			return z;
		}
		void set_race(std::string _race) {
			race = _race;
			str = dex = con = inT = wis = cha = 12;
			if (race == "Human") {
				// ..
			} else if (race == "Elf") {
				str -= 1; dex += 1; con -= 1;
				inT += 1; wis += 0; cha += 0;
			} else if (race == "Half Orc") {
				str += 2; dex -= 1; con += 2;
				inT -= 1; wis -= 1; cha -= 1;
			}
		}

		void get_quirk_info() {

		}

		void add_buff(std::string buff) {

		}

		std::string get_quirk_info(int id) {
			if (id < 0 || quirks.size() <= id) {
				return "out of bound";
			}

			auto [qk, lv] = quirks[id];

			if (qk == "力大") {
				return "力量 + " + std::to_string(lv);
			}
			if (qk == "绵软") {
				return "力量 - " + std::to_string(lv);
			}
			if (qk == "灵巧") {
				return "敏捷 + " + std::to_string(lv);
			}
			if (qk == "迟钝") {
				return "敏捷 - " + std::to_string(lv);
			}
			if (qk == "结实") {
				return "体质 + " + std::to_string(lv);
			}
			if (qk == "不足") {
				return "体质 - " + std::to_string(lv);
			}
			if (qk == "聪明") {
				return "智力 + " + std::to_string(lv);
			}
			if (qk == "笨拙") {
				return "智力 - " + std::to_string(lv);
			}
			if (qk == "敏锐") {
				return "感知 + " + std::to_string(lv);
			}
			if (qk == "涣散") {
				return "感知 - " + std::to_string(lv);
			}
			if (qk == "优雅") {
				return "魅力 + " + std::to_string(lv);
			}
			if (qk == "粗鲁") {
				return "魅力 - " + std::to_string(lv);
			}
			return "no info";
		}

		void add_quirk(std::string quirk, int lv) {
			quirks.push_back({quirk, lv});
			//std::string cmd;
			//cmd = "体弱多病";
			//if (std::equal(cmd.begin(), cmd.end(), buff.begin())) {

			if (quirk == "力大") {
				str += lv;
				return;
			}
			if (quirk == "绵软") {
				str -= lv;
				return;
			}
			if (quirk == "灵巧") {
				dex += lv;
				return;
			}
			if (quirk == "迟钝") {
				dex -= lv;
				return;
			}
			if (quirk == "结实") {
				con += lv;
				return;
			}
			if (quirk == "不足") {
				con -= lv;
				return;
			}
			if (quirk == "聪明") {
				inT += lv;
				return;
			}
			if (quirk == "笨拙") {
				inT -= lv;
				return;
			}
			if (quirk == "敏锐") {
				wis += lv;
				return;
			}
			if (quirk == "涣散") {
				wis -= lv;
				return;
			}
			if (quirk == "优雅") {
				cha += lv;
				return;
			}
			if (quirk == "粗鲁") {
				cha -= lv;
				return;
			}
		}
	};

	struct Elf : Creature {
	};

	struct Ariel : Elf {
		/*
			Literature,
			Science,
			Historiography,
			Martial_Arts,
			Magic,
			Art_of_War,
			Ritual,
			Calendar;
		*/
	};

	Creature& get_Player();
	std::vector<int>& get__A();
	std::vector<int>& get_A();
	std::vector<std::pair<int, int>>& get_empty_grids();
	int get_c0();
	int get_c1();
	void UpdateFOV();

	bool isFOVon();
	void turnon_FOV();
	void turnoff_FOV();

	void teleport_to_lu(std::string who);
	void teleport_to_ld(std::string who);
	void teleport_to_ru(std::string who);
	void teleport_to_rd(std::string who);
	// void teleport_to(std::string map_event_name);

	bool isInFOV(int x, int y);
	bool isExplored(int x, int y);
	void Gen(int c0, int c1);

	void init();
	bool isCmd(std::string cmd);
}
#endif
