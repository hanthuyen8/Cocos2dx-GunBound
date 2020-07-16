#pragma once

#include "cocos2d.h"
#include "SpritePhysics.h"
#include "Character.h"

USING_NS_CC;

constexpr int COLLISION_MASK_CHARACTER = 0x01;;
constexpr int COLLISION_MASK_SPRITE_PHYSICS = 0x02;;

// Class này sẽ đảm nhiệm việc setup mọi thứ lên scene
class GunBoundScene : public Scene
{
public:
	static Scene* createScene();
	virtual bool init() override;
	CREATE_FUNC(GunBoundScene);

	static Vec2 getAcceleration();

private:
	// Tất cả những thứ sẽ setup gồm:
	std::vector<SpritePhysics*> allSpritePhysics{};

	// Background (2 cái)
	Sprite* backgroundLayer1{};
	Sprite* backgroundLayer2{};

	// Terrain (1 cái) kèm physic body
	CC_SYNTHESIZE_READONLY(SpritePhysics*, terrain, SpriteTerrain);

	// Tree (1 cái) kèm physic body
	CC_SYNTHESIZE_READONLY(SpritePhysics*, tree, SpriteTree);

	// Player (1 cái) kèm physic body
	CC_SYNTHESIZE(Character*, player, Player);

	// Các thuộc tính:
	inline static Vec2 acceleration{};

	void addToScene(SpritePhysics* sprite, Vec2&& atPos);

};

