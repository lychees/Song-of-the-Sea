#include <algorithm>
#include <lcf/reader_util.h>
#include "../output.h"
#include "../game_interpreter.h"
#include "../game_map.h"
#include "../game_screen.h"
#include "../game_party.h"
#include "../game_actors.h"
#include "../game_variables.h"
#include "../main_data.h"
#include "../graphics.h"
#include "../spriteset_map.h"
#include "../font.h"
#include "../scene_map.h"
#include "../sprite_character.h"
#include "../game_system.h"
#include "cards.h"
#include "cardsinfo_overlay.h"

#define DO(n) for ( int ____n = n; ____n-->0; )

namespace Cards {

	int caster;
	Instance _;


	void draw_specific::process() {
		for (int i=0;i<_.deck.size();++i) {
			if (_.deck[i].key == key) {
				_.hand.push_back(_.deck[i]);
				_.deck.erase(_.deck.begin() + i);
				return;
			}
		}
	}

	void curse::process() {

		Main_Data::game_screen->ShowBattleAnimation(56, _.battlefield[caster].id, 0);

		for (auto& m: _.battlefield) {
			if (_.battlefield[caster].master != m.master) {
				if (_.battlefield[caster].dist(m) <= 1) {
					m.AP -= lv;
					if (m.AP < 0) m.AP = 0;
				}
			}
		}
	}

	void happy_lady_battlecry::process() {
		for (auto& m: _.battlefield) {
			if (m.key == "harpy_r" || m.key == "harpy_g" || m.key == "harpy_b") {
				m.AP += 1;
				m.DP += 1;
			}
		}
	}
	void happy_lady_deathrattle::process() {
		for (auto& m: _.battlefield) {
			if (m.key == "harpy_r" || m.key == "harpy_g" || m.key == "harpy_b") {
				m.AP -= 1;
				m.DP -= 1;
			}
		}
	}

	monster::monster(std::string key): key(key) {
		auto json = _.json[key];
		cost = json["cost"]; name = std::string(json["name"]);
		hp = json["hp"]; HP = json["HP"]; mp = json["mp"]; MP = json["MP"];
		AP = json["AP"]; DP = json["DP"];
		offset = json["offset"];

		quirks = json["quirks"];

		if (key == "skull") {
			deathrattle.push_back(new draw_specific("skull"));
		}
		if (key == "mummy") {
			deathrattle.push_back(new curse(1));
		}
		if (key == "harpy_r" || key == "harpy_g" || key == "harpy_b") {
			battlecry.push_back(new happy_lady_battlecry());
			deathrattle.push_back(new happy_lady_deathrattle());
		}

		if (name == "ghost") {
			quirks["regeneration"] = 2;
		}
		if (name == "slime" || name == "giant_slime") {
			quirks["regeneration"] = 1;
		}
	}

	Game_Event* monster::ev() const {
		return Game_Map::GetEvent(id);
	}

	int monster::dist(const monster m) {
		int x = ev()->GetX(), y = ev()->GetY();
		int xx = m.ev()->GetX(), yy = m.ev()->GetY();
		return abs(x-xx)+abs(y-yy);
	}
	int monster::enemyNearby() {
		for (int i=0;i<_.battlefield.size();++i) {
			if (_.battlefield[i].master != master) {
				if (dist(_.battlefield[i]) <= 1) return i;
			}
		}
		return -1;
	}
	int monster::enemyInfront() {
		int x = ev()->GetX(), y = ev()->GetY();
		for (int i=0;i<_.battlefield.size();++i) {
			if (_.battlefield[i].master != master) {
				if (x == _.battlefield[i].ev()->GetX()) return i;
				if (y == _.battlefield[i].ev()->GetY()) return i;
			}
		}
		return -1;
	}

	void monster::dead(int i) {
		auto map_event = Game_Map::GetEvent(id); map_event->SetActive(false);
		caster = i;
		for (int i=0;i<deathrattle.size();++i) {
			deathrattle[i]->process();
		}
		deathrattle.clear();
		_.grave.push_back(_.battlefield[i]);
		_.battlefield.erase(_.battlefield.begin() + i);
	}

	void monster::damaged(int d, int aid, int i) {

		Scene_Map* scene = (Scene_Map*)Scene::Find(Scene::Map).get();
		auto s = scene->spriteset->FindCharacter(Game_Map::GetEvent(id));

		Graphics::GetCardsinfoOverlay().DrawFloatText(s->GetX(), s->GetY()-10, Font::ColorDefault, std::to_string(d));

		hp -= d;
		if (hp <= 0) {
			if (aid == 142) aid += 1;
			Main_Data::game_screen->ShowBattleAnimation(aid, id, 0);
			dead(i);
		} else {
			Main_Data::game_screen->ShowBattleAnimation(aid, id, 0);
		}
	}

	int monster::physicalDamaged(int d, int aid, int i) {
		d -= DP; if (d > hp) d = hp;
		if (d > 0) damaged(d, aid, i);
		return d;
	}

	bool monster::checkmate() {
		if (hasQuirk("ranged")) return true;
		int y = ev()->GetY();
		return master == 1 && y == 8 || master == 2 && y == 12;
	}
	void monster::check() {
		if ((master == 1) ^ (_.player_id != 1)) {			
			_.ai_hp -= AP; Main_Data::game_screen->ShowBattleAnimation(142, 6, 0);
			if (_.ai_hp <= 0) Cards::over();			
		} else {
			_.hp -= AP; Main_Data::game_screen->ShowBattleAnimation(142, 10001, 0);
			if (_.hp <= 0) Cards::over();
		}
	}

	void monster::draw() {
		int d = quirks["draw"];
		if (master == 1) {
			DO(d) draw();
		} else {
			DO(d) ai_draw();
		}
		mp = 0;
	}
	void monster::jump_cost() {
		int d = quirks["jump_cost"];
		if (master == 1) {
			_.mp += d;
		} else {
			_.ai_mp += d;
		}
		mp = 0;
	}	

	bool monster::hasQuirk(std::string quirk) {
		return quirks.find(quirk) != quirks.end();
	}

	void monster::atk(int t) {
		int x = ev()->GetX(), xx = _.battlefield[t].ev()->GetX();
		int y = ev()->GetY(), yy = _.battlefield[t].ev()->GetY();

		if (yy < y) {
			ev()->SetFacing(0);
		} else if (yy > y) {
			ev()->SetFacing(2);
		} else if (xx > x) {
			ev()->SetFacing(1);
		} else if (xx < x) {
			ev()->SetFacing(3);
		}
		
		if (hasQuirk(std::string("lifesteal"))) {
			int delta = _.battlefield[t].physicalDamaged(AP, 142, t);
			hp += delta; if (hp > HP) hp = HP;
		} else {
			_.battlefield[t].physicalDamaged(AP, 142, t);
		}
	}

	std::string monster::info() {
		std::string z;
		z += std::to_string(cost) + "??? ";
				z += std::to_string(AP) + "/";
				z += std::to_string(DP) + "/";
				z += std::to_string(HP);
				for (auto& q: quirks) {
					if (q.first == "ranged") z += "????????????";
					else if (q.first == "cavalry") z += "????????????";
					else if (q.first == "flying") z += "????????????";
					else if (q.first == "lifesteal") z += "????????????";
				}
				z += std::string(" ") + std::string(_.json[key]["description"]);
		return z;
	}

	void initJson() {
		if (_.json.empty()) {
			_.json = {
				{
					"wisp", {
						{"name", "?????????"},
						{"cost", 0},
						{"description", ""},
						{"hp", 1},{"HP", 1},
						{"mp", 0},{"MP", 3},
						{"AP", 1},
						{"DP", 0},
						{"charset", "monster-e02"},
						{"offset", 4}
					}
				},
				{
					"slime", {
						{"name", "?????????"},
						{"cost", 1},
						{"description", "?????? 1????????????????????????"},
						{"hp", 2},{"HP", 2},
						{"mp", 0},{"MP", 3},
						{"AP", 1},
						{"DP", 0},
						{"charset", "char_m_sl_g"},
						{"offset", 0},
						{
							"quirks", {
								{"selfhealing", 1}
							}
						}
					}
				},
				{
					"gaint_slime", {
						{"name", "???????????????"},
						{"cost", 5},
						{"description", "?????????????????????????????????????????????"},
						{"hp", 6},{"HP", 6},
						{"mp", 0},{"MP", 3},
						{"AP", 3},
						{"DP", 0},
						{"charset", "char_m_sl_g"},
						{"offset", 2},
						{
							"quirks", {
								{"selfhealing", 1}
							}
						}
					}
				},
				{
					"rock_man", {
						{"name", "?????????"},
						{"cost", 3},
						{"description", "?????????????????????????????????"},
						{"hp", 7},{"HP", 7},
						{"mp", 0},{"MP", 0},
						{"AP", 4},
						{"DP", 0},
						{"charset", "Monster2"},
						{"offset", 4}
					}
				},
				{
					"skull", {
						{"name", "??????"},
						{"cost", 1},
						{"description", "??????????????????????????????????????????"},
						{"hp", 2},{"HP", 2},
						{"mp", 0},{"MP", 0},
						{"AP", 1},
						{"DP", 0},
						{"charset", "Monster1"},
						{"offset", 4}
					}
				},
				{
					"ghost", {
						{"name", "??????"},
						{"cost", 3},
						{"description", "?????? 2"},
						{"hp", 5},{"HP", 5},
						{"mp", 0},{"MP", 0},
						{"AP", 4},
						{"DP", 0},
						{"charset", "Monster1"},
						{"offset", 3},
						{
							"quirks", {
								{"selfhealing", 2}
							}
						}
					}
				},
				{
					"bone_dragon", {
						{"name", "??????"},
						{"cost", 7},
						{"description", "???????????????????????????????????????????????????????????? 1 ???"},
						{"hp", 9},{"HP", 9},
						{"mp", 0},{"MP", 0},
						{"AP", 3},
						{"DP", 0},
						{"charset", "18277"},
						{"offset", 3}
					}
				},
				{
					"mummy", {
						{"name", "?????????"},
						{"cost", 2},
						{"description", "."},
						{"hp", 6},{"HP", 6},
						{"mp", 0},{"MP", 0},
						{"AP", 2},
						{"DP", 0},
						{"charset", "monster-g08"},
						{"offset", 1}
					}
				},
				{
					"elf_dragon", {
						{"name", "?????????"},
						{"cost", 3},
						{"description", ""},
						{"hp", 6},{"HP", 6},
						{"mp", 0},{"MP", 0},
						{"AP", 2},
						{"DP", 0},
						{"charset", "tkool4vip12311"},
						{"offset", 3}
					}
				},
				{
					"griffin", {
						{"name", "??????"},
						{"cost", 3},
						{"description", ""},
						{"hp", 7},{"HP", 7},
						{"mp", 0},{"MP", 0},
						{"AP", 3},
						{"DP", 0},
						{"charset", "22824"},
						{"offset", 6},
						{
							"quirks", {
								{"flying", 1}
							}
						}
					}
				},
				{
					"harpy_r", {
						{"name", "???????????? R"},
						{"cost", 2},
						{"description", ""},
						{"hp", 5},{"HP", 5},
						{"mp", 0},{"MP", 0},
						{"AP", 1},
						{"DP", 0},
						{"charset", "tkool4vip7992"},
						{"offset", 0},
						{
							"quirks", {
								{"flying", 1}
							}
						}
					}
				},
				{
					"harpy_g", {
						{"name", "???????????? G"},
						{"cost", 2},
						{"description", ""},
						{"hp", 5},{"HP", 5},
						{"mp", 0},{"MP", 0},
						{"AP", 1},
						{"DP", 0},
						{"charset", "tkool4vip7992"},
						{"offset", 2},
						{
							"quirks", {
								{"flying", 1}
							}
						}
					}
				},
				{
					"harpy_b", {
						{"name", "???????????? B"},
						{"cost", 2},
						{"description", ""},
						{"hp", 5},{"HP", 5},
						{"mp", 0},{"MP", 0},
						{"AP", 1},
						{"DP", 0},
						{"charset", "tkool4vip7992"},
						{"offset", 4},
						{
							"quirks", {
								{"flying", 1}
							}
						}
					}
				},
				{
					"hydra", {
						{"name", "?????????"},
						{"cost", 7},
						{"description", "????????????"},
						{"hp", 18},{"HP", 18},
						{"mp", 0},{"MP", 0},
						{"AP", 3},
						{"DP", 0},
						{"charset", "22825"},
						{"offset", 2},
						{
							"quirks", {
								{"aoe", 1}
							}
						}
					}
				},
				{
					"angry_chicken", {
						{"name", "???????????????"},
						{"cost", 0},
						{"description", "????????? AP + 5"},
						{"hp", 1},{"HP", 1},
						{"mp", 0},{"MP", 0},
						{"AP", 1},
						{"DP", 0},
						{"charset", "22824"},
						{"offset", 3}
					}
				},
				{
					"grim_reaper", {
						{"name", "??????"},
						{"cost", 6},
						{"description", "?????????????????????????????????????????????????????????????????????????????????????????????????????????????????????"},
						{"hp", 4},{"HP", 4},
						{"mp", 10},{"MP", 10},
						{"AP", 2},
						{"DP", 0},
						{"charset", "??????_??????b"},
						{"offset", 3}
					}
				},
				{
					"succubus", {
						{"name", "??????"},
						{"cost", 6},
						{"description", "10/10 ????????????????????????????????????(10)"},
						{"hp", 4},{"HP", 4},
						{"mp", 5},{"MP", 10},
						{"AP", 1},
						{"DP", 0},
						{"charset", "9890"},
						{"offset", 0}
					}
				},
				{
					"naga", {
						{"name", "??????"},
						{"cost", 6},
						{"description", "."},
						{"hp", 7},{"HP", 7},
						{"mp", 0},{"MP", 0},
						{"AP", 6},
						{"DP", 0},
						{"charset", "monster-g08"},
						{"offset", 3}
					}
				},
				{
					"diamond_men", {
						{"name", "?????????"},
						{"cost", 4},
						{"description", "????????????????????????????????????????????????????????????????????????5%????????????????????????????????????????????????????????????????????????"},
						{"hp", 9},{"HP", 9},
						{"mp", 0},{"MP", 0},
						{"AP", 5},
						{"DP", 0},
						{"charset", "18557"},
						{"offset", 0}
					}
				},
				{
					"manticore", {
						{"name", "??????"},
						{"cost", 6},
						{"description", "?????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????"},
						{"hp", 13},{"HP", 13},
						{"mp", 0},{"MP", 10},
						{"AP", 7},
						{"DP", 0},
						{"charset", "16549"},
						{"offset", 0}
					}
				},
				{
					"green_dragon", {
						{"name", "??????"},
						{"cost", 7},
						{"description", "????????????????????????????????????????????????????????????????????????????????????"},
						{"hp", 15},{"HP", 15},
						{"mp", 0},{"MP", 20},
						{"AP", 8},
						{"DP", 0},
						{"charset", "viptmp1139"},
						{"offset", 0}
					}
				},
				{
					"red_dragon", {
						{"name", "??????"},
						{"cost", 7},
						{"description", "??????????????????????????????????????? 3 ????????????????????????"},
						{"hp", 14},{"HP", 14},
						{"mp", 20},{"MP", 20},
						{"AP", 6},
						{"DP", 1},
						{"charset", "135"},
						{"offset", 0}
					}
				},
				{
					"black_dragon", {
						{"name", "??????"},
						{"cost", 7},
						{"description", "??????????????????????????? 4 ?????????????????????????????????????????????????????????"},
						{"hp", 10},{"HP", 10},
						{"mp", 20},{"MP", 20},
						{"AP", 5},
						{"DP", 1},
						{"charset", "viptmp3278"},
						{"offset", 0}
					}
				},
				{
					"paladin", {
						{"name", "?????????"},
						{"cost", 5},
						{"description", "?????? 2"},
						{"hp", 7},{"HP", 7},
						{"mp", 0},{"MP", 1},
						{"AP", 3},
						{"DP", 1},
						{"charset", "pc?????????"},
						{"offset", 0}
					}
				},
				{
					"silver_knight", {
						{"name", "????????????"},
						{"cost", 6},
						{"description", ""},
						{"hp", 7},{"HP", 7},
						{"mp", 0},{"MP", 1},
						{"AP", 3},
						{"DP", 2},
						{"charset", "viptmp2278"},
						{"offset", 4}
					}
				},
				{
					"death_knight", {
						{"name", "????????????"},
						{"cost", 6},
						{"description", ""},
						{"hp", 9},{"HP", 9},
						{"mp", 0},{"MP", 0},
						{"AP", 3},
						{"DP", 1},
						{"charset", "viptmp2278"},
						{"offset", 1},
						{
							"quirks", {
								{"cavalry", 1}
							}
						}
					}
				},
				{
					"centaur", {
						{"name", "?????????"},
						{"cost", 1},
						{"description", "??????????????? 1 ????????????"},
						{"hp", 3},{"HP", 3},
						{"mp", 0},{"MP", 0},
						{"AP", 1},
						{"DP", 0},
						{"charset", "monster-g04"},
						{"offset", 0},
						{
							"quirks", {
								{"cavalry", 1}
							}
						}
					}
				},
				{
					"unicorn", {
						{"name", "???????????????"},
						{"cost", 6},
						{"description", ""},
						{"hp", 10},{"HP", 10},
						{"mp", 0},{"MP", 0},
						{"AP", 3},
						{"DP", 0},
						{"charset", "???????????????????????????????????????"},
						{"offset", 0},
						{
							"quirks", {
								{"cavalry", 1}
							}
						}
					}
				},
				{
					"valkyrie", {
						{"name", "?????????"},
						{"cost", 7},
						{"description", ""},
						{"hp", 15},{"HP", 15},
						{"mp", 0},{"MP", 0},
						{"AP", 8},
						{"DP", 0},
						{"charset", "16429"},
						{"offset", 0}
					}
				},
				{
					"witch", {
						{"name", "??????"},
						{"cost", 1},
						{"description", "5/5: ?????? 1d6 ?????????5???"},
						{"hp", 6},{"HP", 6},
						{"mp", 10},{"MP", 10},
						{"AP", 1},
						{"DP", 0},
						{"charset", "monster-g04"},
						{"offset", 5},
						{
							"quirks", {
								{"ranged", 1}
							}
						}
					}
				},
				{
					"priest", {
						{"name", "??????"},
						{"cost", 5},
						{"description", "10/10 ????????????????????????????????? +1/2???10???"},
						{"hp", 6},{"HP", 6},
						{"mp", 10},{"MP", 10},
						{"AP", 2},
						{"DP", 0},
						{"charset", "1541"},
						{"offset", 0},
						{
							"quirks", {
								{"ranged", 1}
							}
						}
					}
				},
				{
					"nec", {
						{"name", "????????????"},
						{"cost", 5},
						{"description", "5/5 ????????????????????????????????????????????????5???"},
						{"hp", 6},{"HP", 6},
						{"mp", 5},{"MP", 5},
						{"AP", 2},
						{"DP", 0},
						{"charset", "c???????????????_???"},
						{"offset", 0},
						{
							"quirks", {
								{"ranged", 1}
							}
						}
					}
				},
				{
					"stone_gargoyle", {
						{"name", "?????????"},
						{"cost", 2},
						{"description", "?????????????????????"},
						{"hp", 2},{"HP", 2},
						{"mp", 0},{"MP", 0},
						{"AP", 2},
						{"DP", 1},
						{"charset", "18550"},
						{"offset", 0}
					}
				},
				{
					"pikeman", {
						{"name", "??????"},
						{"cost", 1},
						{"description", ""},
						{"hp", 3},{"HP", 3},
						{"mp", 0},{"MP", 0},
						{"AP", 2},
						{"DP", 0},
						{"charset", "heisic2"},
						{"offset", 1}
					}
				},
				{
					"halberdier", {
						{"name", "??????"},
						{"cost", 2},
						{"description", ""},
						{"hp", 5},{"HP", 5},
						{"mp", 0},{"MP", 0},
						{"AP", 3},
						{"DP", 0},
						{"charset", "6733"},
						{"offset", 5}
					}
				},
				{
					"snail", {
						{"name", "?????????"},
						{"cost", 1},
						{"description", "?????? 1???"},
						{"hp", 2},{"HP", 2},
						{"mp", 0},{"MP", 0},
						{"AP", 1},
						{"DP", 1},
						{"charset", "?????????????????????"},
						{"offset", 0}
					}
				},
				{
					"magma_snail", {
						{"name", "????????????"},
						{"cost", 2},
						{"description", "?????? 1???"},
						{"hp", 4},{"HP", 4},
						{"mp", 0},{"MP", 0},
						{"AP", 2},
						{"DP", 1},
						{"charset", "?????????????????????"},
						{"offset", 2}
					}
				},
				{
					"crusader", {
						{"name", "?????????"},
						{"cost", 4},
						{"description", "?????? 1???"},
						{"hp", 6},{"HP", 6},
						{"mp", 0},{"MP", 0},
						{"AP", 3},
						{"DP", 1},
						{"charset", "23178"},
						{"offset", 1}
					}
				},
				{
					"cavalier", {
						{"name", "??????"},
						{"cost", 6},
						{"description", "?????????????????????????????????"},
						{"hp", 12},{"HP", 12},
						{"mp", 0},{"MP", 0},
						{"AP", 3},
						{"DP", 0},
						{"charset", "heisic5"},
						{"offset", 0},
						{
							"quirks", {
								{"cavalry", 1}
							}
						}
					}
				},
				{
					"bat", {
						{"name", "??????"},
						{"cost", 2},
						{"description", ""},
						{"hp", 4},{"HP", 4},
						{"mp", 0},{"MP", 0},
						{"AP", 1},
						{"DP", 0},
						{"charset", "17708"},
						{"offset", 1},
						{
							"quirks", {
								{"flying", 1},
								{"lifesteal", 1}
							}
						}
					}
				},
				{
					"vampire", {
						{"name", "?????????"},
						{"cost", 4},
						{"description", ""},
						{"hp", 7},{"HP", 7},
						{"mp", 0},{"MP", 0},
						{"AP", 3},
						{"DP", 0},
						{"charset", "viptmp2278"},
						{"offset", 0},
						{
							"quirks", {
								{"lifesteal", 1}
							}
						}
					}
				},
				{
					"farmer_john", {
						{"name", "????????????"},
						{"cost", 1},
						{"description", "0/10 ???????????????10???"},
						{"hp", 2},{"HP", 2},
						{"mp", 0},{"MP", 10},
						{"AP", 1},
						{"DP", 0},
						{"charset", "c????????????"},
						{"offset", 2}
					}
				},
				{
					"werewolf", {
						{"name", "??????"},
						{"cost", 5},
						{"description", ""},
						{"hp", 11},{"HP", 11},
						{"mp", 0},{"MP", 10},
						{"AP", 6},
						{"DP", 0},
						{"charset", "c????????????"},
						{"offset", 0}
					}
				},
				{
					"brown_bear", {
						{"name", "??????"},
						{"cost", 4},
						{"description", ""},
						{"hp", 9},{"HP", 9},
						{"mp", 0},{"MP", 10},
						{"AP", 5},
						{"DP", 0},
						{"charset", "tkool4vip9234"},
						{"offset", 1}
					}
				}
			};
		};
	}

	void over() {
		_.pause = true;
		if (_.ai_hp <= 0) {
			Main_Data::game_system->BgmPlay(Main_Data::game_system->GetSystemBGM(Main_Data::game_system->BGM_Victory));
		} else {
			Main_Data::game_system->BgmPlay(Main_Data::game_system->GetSystemBGM(Main_Data::game_system->BGM_GameOver));
		}
	}

	void pvpDualInit() {
		Output::Debug("Dual Start");
		_.pause = false;
		_.deck.clear(); _.ai_deck.clear();
		_.hand.clear(); _.ai_hand.clear();
		_.battlefield.clear();

		_.mp = _.MP = _.ai_mp = _.ai_MP = _.turn = 0;
		_.hp = _.ai_hp = 20;

		// Init Player Deck
		std::vector<int> party_items;
		Main_Data::game_party->GetItems(party_items);		

		for (size_t i = 0; i < party_items.size(); ++i) {
			auto item = *lcf::ReaderUtil::GetElement(lcf::Data::items, party_items[i]);
			std::string s = std::string(item.name);
			if (s.substr(0, 5) != ".card") continue;
			s = s.substr(6);
			int cnt = Main_Data::game_party->GetItemCount(party_items[i]);
			DO(cnt) _.deck.push_back(monster(s));
		}

		DO(5) draw();
		Graphics::setCardsInfo(true);
	}

	void initP1(){
		_.player_id = 1;
	}

	void initP2(){
		_.player_id = 2;
	}

	void init() {

		_.pause = false;
		_.deck.clear(); _.ai_deck.clear();
		_.hand.clear(); _.ai_hand.clear();
		_.battlefield.clear();

		_.mp = _.MP = _.ai_mp = _.ai_MP = _.turn = 0;
		_.hp = _.ai_hp = 20;

		// Init Player Deck
		std::vector<int> party_items;
		Main_Data::game_party->GetItems(party_items);

		for (size_t i = 0; i < party_items.size(); ++i) {
			auto item = *lcf::ReaderUtil::GetElement(lcf::Data::items, party_items[i]);
			std::string s = std::string(item.name);
			if (s.substr(0, 5) != ".card") continue;
			s = s.substr(6);
			int cnt = Main_Data::game_party->GetItemCount(party_items[i]);
			DO(cnt) _.deck.push_back(monster(s));
		}

		// Init Enemy Deck
		DO(20) {
			int i = 100 + rand() % 21;
			auto item = *lcf::ReaderUtil::GetElement(lcf::Data::items, i);
			std::string s = std::string(item.name);
			if (s.substr(0, 5) != ".card") continue;
			s = s.substr(6);
			_.ai_deck.push_back(monster(s));
		}

		DO(7) draw(); DO(7) ai_draw();
		Graphics::setCardsInfo(true);
	}

	void show() {
		lcf::rpg::EventCommand com;
		//com.string = (std::string("?????? ") + std::to_string(_.current_map_event_id)).c_str();
		// com.string = lcf::DBString(std::to_string(_.current_map_event_id));

		for (int i=0;i<_.battlefield.size();++i) {
			// Output::Debug("id,id: {} {}", _.p1[i].id, _.current_map_event_id);
			if (_.battlefield[i].id == _.current_map_event_id) {
				com.string = lcf::DBString(_.battlefield[i].info());
				break;
			}
		}

		auto t = Game_Map::GetInterpreter();
		t.CommandShowMessage(com);
	}

	int getBattleFieldId(int id) {
		for (int i=0;i<_.battlefield.size();++i) {
			if (_.battlefield[i].id == id) {
				return i;
			}
		}
		return -1;
	}

	void ai_draw() {
		if (_.ai_deck.empty()) return;
		int t = rand() % _.ai_deck.size();
		_.ai_hand.push_back(_.ai_deck[t]);
		_.ai_deck.erase(_.ai_deck.begin() + t);
	}

	void draw() {
		if (_.deck.empty()) return;
		int t = rand() % _.deck.size();
		_.hand.push_back(_.deck[t]);
		_.deck.erase(_.deck.begin() + t);
	}

	void ai_turn() {
		// std::random_shuffle(_.ai_hand.begin(), _.ai_hand.end()); //?
		for (int i=0;i<_.ai_hand.size();++i) {
			if (_.ai_hand[i].cost <= _.ai_mp) {
				_.ai_mp -= _.ai_hand[i].cost;
				Game_Map::summon(_.ai_hand[i], 2, 8 + rand() % 5, 8);
				_.ai_hand.erase(_.ai_hand.begin() + i);
			}
		}
	}

	void mainLoop() {
		if (_.pause) return;
		draw(); ai_draw();
		_.turn += 1;
		_.mp = _.MP = _.turn;
		_.ai_mp = _.ai_MP = _.turn;

		for (auto &m: _.battlefield) {
			if (m.mp < m.MP) m.mp += 1;
		}

		ai_turn();
		// Output::Debug("main loop: {} {}", _.turn, _.mp);
	}

	void changeAvatar() {
		Game_Actor* actor = Main_Data::game_actors->GetActor(1);
		auto file = ToString(Main_Data::game_actors->GetActor(14)->GetName());
		int idx = int(Main_Data::game_variables->Get(1));
		actor->SetSprite(file, idx, 0);
		Main_Data::game_player->ResetGraphic();
	}

	void prevCard() {
		if (!_.hand.empty()) {
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Cursor));
			_.hand.insert(_.hand.begin(), _.hand.back());
			_.hand.pop_back();
		} else {
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Buzzer));
		}
	}
	void succCard() {
		if (!_.hand.empty()) {
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Cursor));
			_.hand.push_back(_.hand[0]);
			_.hand.erase(_.hand.begin());
		} else {
			Main_Data::game_system->SePlay(Main_Data::game_system->GetSystemSE(Main_Data::game_system->SFX_Buzzer));
		}
	}

	bool isCmd(std::string msg) {
		std::string cmd;

		if (msg == ".mainLoop") {
			mainLoop();
			return true;
		}
		if (msg == ".pvpDualInit") {
			pvpDualInit();
			return true;
		}
		if (msg == ".dualInit") {
			init();
			return true;
		}
		if (msg == ".initP1") {
			initP1();
			return true;
		}
		if (msg == ".initP2") {
			initP2();
			return true;
		}
		if (msg == ".changeAvatar") {
			changeAvatar();
			return true;
		}
		if (msg == ".showSpiritsStatus") {
			show();
			return true;
		}

		cmd = ".summon2";
		if (std::equal(cmd.begin(), cmd.end(), msg.begin())) {			
			std::istringstream iss(msg); std::string _, key; int p_id, x, y, flag, hp, HP, AP, mp, MP, offset; iss >> _ >> key >> flag >> p_id >> x >> y >> hp >> HP >> AP >> mp >> MP >> offset;
			if(flag == 0){
				Game_Map::summon(monster(key), p_id, x, y);
			} else {
				auto monster_sum = monster(key);
				monster_sum.hp = hp; monster_sum.HP = HP; monster_sum.AP = AP; monster_sum.mp = mp; monster_sum.MP = MP; monster_sum.offset = offset;
				Game_Map::summon(monster_sum, p_id, x, y);
			}
			return true;
		}

		cmd = ".summon";
		if (std::equal(cmd.begin(), cmd.end(), msg.begin())) {
			std::istringstream iss(msg); std::string _; int p_id, x, y; iss >> _ >> p_id >> x >> y;
			Game_Map::newMapEvent("MonsterTemplate", p_id, x, y);
			return true;
		}		

		return false;
	}	


	Instance& instance() {
		return _;
	}

} // namespace Cards

#undef DO
