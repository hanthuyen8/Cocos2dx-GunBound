#pragma once

#include "cocos2d.h"
using Vec2 = cocos2d::Vec2;

namespace EventNames
{
	const std::string PLAYER_MOVEMENT = "Player_Movement";
	const std::string PLAYER_AIM = "Player_Aim";
	const std::string PLAYER_CHARGE = "Player_Charge";
	const std::string PLAYER_FIRE = "Player_Fire";
}

namespace AnimSuffix
{
	const std::string IDLE = "/Idle/";
	const std::string ATTACK = "/Attack/";
	const std::string WALK = "/Walk/";
}

namespace Map1
{
	const std::string MAP1_FILE_TMX{ "GunBound/TileMaps/Map1.tmx" };
	const Vec2 MAP1_POSITION{ 38.842, 132.818 };
	const Vec2 MAP1_ANCHOR{ 0, 0 };
	const std::vector<Vec2> MAP1_POLYLINE{
		Vec2{652, 702}, Vec2{644, 691}, Vec2{630, 701}, Vec2{626, 689}, Vec2{616, 689}, Vec2{600, 634},
		Vec2{581, 637}, Vec2{577, 588}, Vec2{586, 561}, Vec2{625, 556}, Vec2{622, 528},
		Vec2{528, 528}, Vec2{528, 576}, Vec2{234, 576}, Vec2{240, 611}, Vec2{233, 624},
		Vec2{197, 622}, Vec2{198, 576}, Vec2{0, 576}, Vec2{0, 39}, Vec2{40, 0},
		Vec2{1160, 0}, Vec2{1200, 39}, Vec2{1200, 336}, Vec2{1147, 336}, Vec2{1152, 384},
		Vec2{1056, 384}, Vec2{1060, 336}, Vec2{1008, 336}, Vec2{1008, 384}, Vec2{903, 384},
		Vec2{904, 415}, Vec2{890, 462}, Vec2{876, 447}, Vec2{868, 470}, Vec2{854, 449},
		Vec2{845, 465}, Vec2{830, 437}, Vec2{831, 387}, Vec2{768, 384}, Vec2{768, 528},
		Vec2{670, 528}, Vec2{670, 555}, Vec2{711, 558}, Vec2{718, 597}, Vec2{700, 623},
		Vec2{697, 672}, Vec2{659, 689}
	};

	const Vec2 CHARACTER_POSITION_1{ 161.531, 810.164 };
}

namespace Characters
{
	struct AnimConfig
	{
		std::string name;
		byte totalFrame;
		float fps;
	};
	const std::string STEAM_MAN_NAME{ "Character_SteamMan" };
	const std::string STEAM_MAN_FILE_PLIST{ "GunBound/Animations/Character_SteamMan.plist" };
	const Vec2 STEAM_MAN_ANCHOR{ 0.34, 0.25 };
	const float STEAM_MAN_CIRCLE_COL_RADIUS{ 32 };
	const AnimConfig STEAM_MAN_ANIM_IDLE{ STEAM_MAN_NAME + AnimSuffix::IDLE, 4, 1/4.0f };
	const AnimConfig STEAM_MAN_ANIM_ATTACK{ STEAM_MAN_NAME + AnimSuffix::ATTACK, 6, 1.5f/6.0f };
	const AnimConfig STEAM_MAN_ANIM_WALK{ STEAM_MAN_NAME + AnimSuffix::WALK, 6, 1.5f/6.0f };
}