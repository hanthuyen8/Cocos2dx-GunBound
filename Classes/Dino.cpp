#include "Dino.h"

namespace
{
	// Helper
	Vector<SpriteFrame*> getSpriteFramesFromCache(std::string_view dinoName, std::pair<int, int> sfRange)
	{
		const auto name = std::string{ dinoName } + "-";
		const auto cache = SpriteFrameCache::getInstance();
		Vector<SpriteFrame*> listAnimIdle{};

		listAnimIdle.reserve(sfRange.second - sfRange.first + 1);
		for (int i{ sfRange.first }; i <= sfRange.second; i++)
		{
			const auto sf = cache->getSpriteFrameByName(name + std::to_string(i) + ".png");
			if (sf)
				listAnimIdle.pushBack(sf);
		}
		return listAnimIdle;
	}
}

Dino::Dino(std::string_view dinoName)
{
	auto cache = SpriteFrameCache::getInstance();
	const auto name = std::string{ dinoName };
	const auto path = "Dino\\" + name;
	cache->addSpriteFramesWithFile(path + ".plist", path + ".png");

	Vector<SpriteFrame*> listAnimIdle = getSpriteFramesFromCache(dinoName, sfRangeIdle);
	Vector<SpriteFrame*> listAnimMove = getSpriteFramesFromCache(dinoName, sfRangeMove);

	idleAnimation = Animation::createWithSpriteFrames(listAnimIdle, idleFps);
	moveAnimation = Animation::createWithSpriteFrames(listAnimMove, moveFps);
	sprite = Sprite::createWithSpriteFrame(listAnimIdle.at(0));
}
