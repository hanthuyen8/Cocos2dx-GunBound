#include "GunBoundScene.h"
#include "SceneSetup.h"
#include "Helper.h"
#include "Ammo.h"
#include "CustomAnimation.h"

// Resource Paths
const auto PATH_EMPTY = "Empty.png";
const auto PATH_BG_LAYER1 = "GunBound/BackgroundLayer1.png";
const auto PATH_BG_LAYER2 = "GunBound/BackgroundLayer2.png";
const auto PATH_TERRAIN1 = "GunBound/Terrain1.png";
const auto PATH_TERRAIN2 = "GunBound/Terrain2.png";
const auto PATH_TREE = "GunBound/Tree.png";
const std::string CHARACTER_STEAM_MAN = "Character_SteamMan";

const auto GRAVITY = 981;
const auto LEVEL_WIDTH = 2553;
float cameraMinY = 0;
float cameraMinX = 0;
float cameraMaxX = 0;

namespace
{
	Animation* createAnimation(SpriteFrameCache* cache, std::string_view namePattern, int number)
	{
		Animation* anim = Animation::create();
		std::string pattern{ namePattern };
		for (int i{}; i < number; i++)
		{
			const auto sf = cache->getSpriteFrameByName(pattern + std::to_string(i));
			CC_ASSERT(sf);
			anim->addSpriteFrame(sf);
		}
		return anim;
	}
}

Scene* GunBoundScene::createScene()
{
	return GunBoundScene::create();
}

bool GunBoundScene::init()
{
	if (!Scene::initWithPhysics())
		return false;

#pragma region Spritesheets & Animations

	const auto spriteCache = SpriteFrameCache::getInstance();
	const auto animationCache = AnimationCache::getInstance();

	spriteCache->addSpriteFramesWithFile("GunBound/Animations/Character_SteamMan.plist");
	CustomAnimation::addPlistFile("GunBound/Animations/AnimationSmoke.plist", "AnimationSmoke");

	const auto steamManIdleAnim = createAnimation(spriteCache, "Character_SteamMan/Idle/", 4);
	animationCache->addAnimation(steamManIdleAnim, CHARACTER_STEAM_MAN + "_Idle");

	const auto steamManAttackAnim = createAnimation(spriteCache, "Character_SteamMan/Attack/", 6);
	animationCache->addAnimation(steamManAttackAnim, CHARACTER_STEAM_MAN + "_Attack");

	const auto steamManWalkAnim = createAnimation(spriteCache, "Character_SteamMan/Walk/", 6);
	animationCache->addAnimation(steamManWalkAnim, CHARACTER_STEAM_MAN + "_Walk");

#pragma endregion

#pragma region Collisions

	Ammo::COLLISION_CATEGORY = COLLISION_MASK_AMMO;
	Ammo::COLLISION_WITH = COLLISION_MASK_CHARACTER | COLLISION_MASK_SPRITE_PHYSICS;

	Character::COLLISION_CATEGORY = COLLISION_MASK_CHARACTER;
	Character::COLLISION_WITH = COLLISION_MASK_SPRITE_PHYSICS | COLLISION_MASK_AMMO;

	SpritePhysics::COLLISION_CATEGORY = COLLISION_MASK_SPRITE_PHYSICS;
	SpritePhysics::COLLISION_WITH = COLLISION_MASK_CHARACTER | COLLISION_MASK_AMMO;

	const auto collisionListener = EventListenerPhysicsContact::create();
	collisionListener->onContactBegin = CC_CALLBACK_1(GunBoundScene::physicsCollisionFilterRules, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(collisionListener, this);

#pragma endregion

	// Get window size
	const auto director = Director::getInstance();
	const auto winSize = director->getVisibleSize();
	this->setPosition(winSize.width / 2, winSize.height / 2);

	//------------------------------------------------------------------------------------------------------
	// Tất cả những thứ sẽ setup gồm:
	//------------------------------------------------------------------------------------------------------

	GunBoundScene::acceleration = Vec2{ 0, -9.81f };
	this->getPhysicsWorld()->setGravity(Vec2{ 0,-GRAVITY });

	// Background (2 cái)
	backgroundLayer = BackgroundLayer::create();
	this->addChild(backgroundLayer);

	const auto background1 = Sprite::create(PATH_BG_LAYER2);
	CC_ASSERT(background1);
	backgroundLayer->addChild(background1);
	background1->setPosition(-563.241, -26.082);

	const auto background2 = Sprite::create(PATH_BG_LAYER2);
	CC_ASSERT(background2);
	backgroundLayer->addChild(background2);
	background2->setPosition(936.93, -26.082);

	// Tile Map

	const auto tileMap = TMXTiledMap::create(Map1::MAP1_FILE_PATH);
	CC_ASSERT(tileMap);
	tileMap->setName("Map1");



	//// Terrain (2 cái) kèm physic body
	//const auto terrain1 = SpritePhysics::createInstance(PATH_TERRAIN1, mapbox::getTrianglesFromPolyline({
	//	Vec2{96,120},Vec2{-283,32},Vec2{-434,-40},Vec2{-640,-40},Vec2{-640,-120},Vec2{640,-120},Vec2{640,-40},Vec2{565,-40},Vec2{264,-8},
	//	}));
	//CC_ASSERT(terrain1);
	//terrain1->setName("Terrain1");
	//addToScene(terrain1, Vec2{ 1.865, -238.039 });

	//const auto terrain2 = SpritePhysics::createInstance(PATH_TERRAIN2, mapbox::getTrianglesFromPolyline({
	//	Vec2{-640,76},Vec2{-640,-75},Vec2{640,-75},Vec2{640,76},Vec2{498,76},Vec2{247,4},Vec2{-97,4},Vec2{-467,76}
	//	}));
	//CC_ASSERT(terrain2);
	//terrain1->setName("Terrain2");
	//addToScene(terrain2, Vec2{ 1282.27, -354.47 });

	//// Tree (1 cái) kèm physic body
	//const auto tree = SpritePhysics::createInstance(PATH_TREE, mapbox::getTrianglesFromPolyline({
	//	Vec2{ -13,155 }, Vec2{ -38,139 }, Vec2{ -45,122 }, Vec2{ -67,116 }, Vec2{ -77,93 }, Vec2{ -106,83 }, Vec2{ -119,60 }, Vec2{ -113,27 }, Vec2{ -93,12 }, Vec2{ -100,-27 }, Vec2{ -81,-36 }, Vec2{ -77,-61 }, Vec2{ -59,-62 }, Vec2{ -56,-92 }, Vec2{ -31,-98 }, Vec2{ -31,-137 }, Vec2{ -57,-137 }, Vec2{ -69,-155 }, Vec2{ 54,-155 }, Vec2{ 46,-139 }, Vec2{ 27,-137 }, Vec2{ 27,-84 }, Vec2{ 46,-81 }, Vec2{ 49,-59 }, Vec2{ 72,-58 }, Vec2{ 75,-44 }, Vec2{ 112,-20 }, Vec2{ 111,-3 }, Vec2{ 95,6 }, Vec2{ 119,34 }, Vec2{ 113,68 }, Vec2{ 86,85 }, Vec2{ 57,133 }
	//	}));
	//CC_ASSERT(tree);
	//tree->setName("Tree");
	//addToScene(tree, Vec2{ -570, -140 });

	// Player (1 cái) kèm physic body
	player = Character::create(CHARACTER_STEAM_MAN, Vec2{ 0.34f, 0.25f }, Size{ 128,128 }, 32);
	CC_ASSERT(player);

	player->setMoveSpeed(450);
	player->setPosition(-537, 100);
	player->listenToKeyboardMovement();
	player->setName("Player");
	player->setOnCharacterMovementCallback(CC_CALLBACK_1(GunBoundScene::moveCameraByCharacter, this));
	this->addChild(player);

#pragma region Camera

	camera = getDefaultCamera();
	director->setClearColor(Color4F(170.0f / 255, 124.0f / 255, 82.0f / 255, 1));
	const auto camPos = camera->getPosition();
	cameraMinY = camPos.y;
	cameraMinX = winSize.width / 2;
	cameraMaxX = cameraMinX + LEVEL_WIDTH - winSize.width;

#pragma endregion

	return true;
}

Vec2 GunBoundScene::getAcceleration()
{
	return GunBoundScene::acceleration;
}

void GunBoundScene::addToScene(SpritePhysics* sprite, Vec2&& atPos)
{
	this->addChild(sprite);
	sprite->setPosition(atPos);
	allSpritePhysics.push_back(sprite);
}

bool GunBoundScene::physicsCollisionFilterRules(PhysicsContact& contact)
{
	const auto shapeA = contact.getShapeA();
	const auto shapeB = contact.getShapeB();

	if (((shapeA->getCategoryBitmask() & shapeB->getContactTestBitmask()) == 0) ||
		((shapeA->getContactTestBitmask() & shapeB->getCategoryBitmask()) == 0))
		return false;

	return true;
}

void GunBoundScene::moveCameraByCharacter(Vec2 characterWPos)
{
	// Camera sẽ di chuyển theo vị trí của Character
	// Character sẽ luôn nằm giữa màn hình, nhưng có ngoại lệ:
	// 1. Biên bên trái, bên phải của màn hình sẽ không thể vượt qua.
	// 2. Biên bên dưới sẽ không thể vượt qua.

	if (characterWPos.y < cameraMinY)
		characterWPos.y = cameraMinY;

	characterWPos.x = clampf(characterWPos.x, cameraMinX, cameraMaxX);

	camera->setPosition(characterWPos);
}
