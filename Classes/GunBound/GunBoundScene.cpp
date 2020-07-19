#include "GunBoundScene.h"
#include "Helper.h"
#include "Ammo.h"
#include "CustomAnimation.h"

// Resource Paths
const auto PATH_EMPTY = "Empty.png";
const auto PATH_BG_LAYER1 = "GunBound/BackgroundLayer1.png";
const auto PATH_BG_LAYER2 = "GunBound/BackgroundLayer2.png";
const auto PATH_TERRAIN = "GunBound/Terrain.png";
const auto PATH_TREE = "GunBound/Tree.png";
const auto PATH_PLAYER = "GunBound/Player.png";
const auto PATH_ANIM_SMOKE = "GunBound/Animations/AnimationSmoke.plist";

Scene* GunBoundScene::createScene()
{
	return GunBoundScene::create();
}

bool GunBoundScene::init()
{
	if (!Scene::initWithPhysics())
		return false;

	// Add SpriteSheet
	CustomAnimation::addPlistFile("GunBound/Animations/AnimationSmoke.plist", "AnimationSmoke");

	// Set collision masks
	Ammo::COLLISION_CATEGORY = COLLISION_MASK_AMMO;
	Ammo::COLLISION_WITH = COLLISION_MASK_CHARACTER | COLLISION_MASK_SPRITE_PHYSICS;

	Character::COLLISION_CATEGORY = COLLISION_MASK_CHARACTER;
	Character::COLLISION_WITH = COLLISION_MASK_SPRITE_PHYSICS | COLLISION_MASK_AMMO;

	SpritePhysics::COLLISION_CATEGORY = COLLISION_MASK_SPRITE_PHYSICS;
	SpritePhysics::COLLISION_WITH = COLLISION_MASK_CHARACTER | COLLISION_MASK_AMMO;

	const auto collisionListener = EventListenerPhysicsContact::create();
	collisionListener->onContactBegin = CC_CALLBACK_1(GunBoundScene::physicsCollisionFilterRules, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(collisionListener, this);

	// Get window size
	const auto winSize = Director::getInstance()->getVisibleSize();
	this->setPosition(winSize.width / 2, winSize.height / 2);

	// Tất cả những thứ sẽ setup gồm:
	GunBoundScene::acceleration = Vec2{ 0, -9.81f };

	// Background (2 cái)
	backgroundLayer1 = Sprite::create(PATH_BG_LAYER1);
	RETURN_FALSE_IF_NULL_PTR(backgroundLayer1, "GunBoundScene backgroundLayer1");
	this->addChild(backgroundLayer1);

	backgroundLayer2 = Sprite::create(PATH_BG_LAYER2);
	RETURN_FALSE_IF_NULL_PTR(backgroundLayer2, "GunBoundScene backgroundLayer2");
	this->addChild(backgroundLayer2);

	// Terrain (1 cái) kèm physic body
	terrain = SpritePhysics::createInstance(PATH_TERRAIN, Helper::getTrianglesFromPolyline({
		Vec2{96,120},Vec2{-283,32},Vec2{-434,-40},Vec2{-640,-40},Vec2{-640,-120},Vec2{640,-120},Vec2{640,-40},Vec2{565,-40},Vec2{264,-8},
		}));
	RETURN_FALSE_IF_NULL_PTR(terrain, "GunBoundScene terrain");
	terrain->setName("Terrain");
	addToScene(terrain, Vec2{ 0, -238 });

	// Tree (1 cái) kèm physic body
	tree = SpritePhysics::createInstance(PATH_TREE, Helper::getTrianglesFromPolyline({
		Vec2{ -13,155 }, Vec2{ -38,139 }, Vec2{ -45,122 }, Vec2{ -67,116 }, Vec2{ -77,93 }, Vec2{ -106,83 }, Vec2{ -119,60 }, Vec2{ -113,27 }, Vec2{ -93,12 }, Vec2{ -100,-27 }, Vec2{ -81,-36 }, Vec2{ -77,-61 }, Vec2{ -59,-62 }, Vec2{ -56,-92 }, Vec2{ -31,-98 }, Vec2{ -31,-137 }, Vec2{ -57,-137 }, Vec2{ -69,-155 }, Vec2{ 54,-155 }, Vec2{ 46,-139 }, Vec2{ 27,-137 }, Vec2{ 27,-84 }, Vec2{ 46,-81 }, Vec2{ 49,-59 }, Vec2{ 72,-58 }, Vec2{ 75,-44 }, Vec2{ 112,-20 }, Vec2{ 111,-3 }, Vec2{ 95,6 }, Vec2{ 119,34 }, Vec2{ 113,68 }, Vec2{ 86,85 }, Vec2{ 57,133 }
		}));
	RETURN_FALSE_IF_NULL_PTR(tree, "GunBoundScene tree");
	tree->setName("Tree");
	addToScene(tree, Vec2{ -570, -140 });

	// Player (1 cái) kèm physic body
	player = Character::create(PATH_PLAYER, 55);
	RETURN_FALSE_IF_NULL_PTR(player, "GunBoundScene player");

	player->setAnchorPoint(Vec2{ 0.45f, 0.5f });
	player->setPosition(-537, 63);
	player->listenToKeyboardMovement();
	player->setName("Player");
	this->addChild(player);

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
