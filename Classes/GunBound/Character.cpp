#include "Character.h"
#include "Helper.h"
#include <algorithm>
#include "SpritePhysics.h"

const int FALL_SPEED = 981;

Character* Character::create(std::string_view fileName, float radius, Vec2 anchor)
{
	Character* instance = new(std::nothrow) Character();
	if (instance && instance->init(fileName, radius, anchor))
	{
		instance->autorelease();
		return instance;
	}

	CC_SAFE_DELETE(instance);
	return nullptr;
}

bool Character::init(std::string_view fileName, float radius, Vec2 anchor)
{
	if (!Node::init())
		return false;

	this->radius = radius;

	sprite = Sprite::create(std::string{ fileName });
	CC_ASSERT(sprite);
	sprite->setAnchorPoint(anchor);
	this->addChild(sprite);

	physicsBody = PhysicsBody::create();
	physicsBody->addShape(PhysicsShapeCircle::create(radius, PhysicsMaterial::PhysicsMaterial(0, 0, 0)));
	physicsBody->setDynamic(true);
	physicsBody->setGravityEnable(false);
	physicsBody->setRotationEnable(true);

	physicsBody->setCategoryBitmask(COLLISION_CATEGORY);
	physicsBody->setContactTestBitmask(COLLISION_WITH);

	const auto collisionListener = EventListenerPhysicsContact::create();
	collisionListener->onContactPreSolve = CC_CALLBACK_2(Character::onCollisionEnter, this);
	collisionListener->onContactSeparate = CC_CALLBACK_1(Character::onCollisionExit, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(collisionListener, this);

	this->setPhysicsBody(physicsBody);

	cannon = Cannon::create(radius);
	CC_ASSERT(cannon);

	cannon->setPosition(this->convertToNodeSpace(Vec2::ZERO));
	this->addChild(cannon);

	return true;
}

void Character::update(float dt)
{
	const auto groundDistance = findGroundDistanceAndNormal();
	sprite->setRotation(clampf(angle, -90, 90));

	if (moveHorizontal != 0)
	{
		// Khi Character có lệnh di chuyển

		const auto velocity = physicsBody->getVelocity();
		sprite->setFlippedX(moveHorizontal < 0);

		// Gravity nghiêng theo mặt đất
		auto gravity = FALL_SPEED * -1 * groundNormal;
		physicsBody->applyForce(gravity);

		// clamp velocity.x không cho vượt quá moveSpeed
		auto velocity_x = std::abs(velocity.x);

		if (isFirstMove)
			velocity_x = (moveSpeed * 10 - velocity_x) * moveHorizontal;
		else
			velocity_x = (moveSpeed - velocity_x) * moveHorizontal;

		physicsBody->applyForce(Vec2{ velocity_x ,0 });

		isFirstMove = false;
	}
	else
	{
		// Khi Character không có lệnh di chuyển -> đứng yên
		isFirstMove = true;

		if (!isTouchGround && groundDistance > 2)
		{
			// Gravity hút xuống đất
			auto gravity = FALL_SPEED * -1 * Vec2::UNIT_Y;
			physicsBody->applyForce(gravity);
		}
		else
		{
			physicsBody->setVelocity(Vec2::ZERO);
		}
	}
}

void Character::onKeyPressed(EventKeyboard::KeyCode key, Event*)
{
	moveHorizontal = 0;
	isFireAndStopMoving = false;

	switch (key)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
		isFireAndStopMoving = true;
		break;

	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		moveHorizontal = -1;
		break;

	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		moveHorizontal = 1;
		break;

	default:
		break;
	}
}

void Character::listenToKeyboardMovement()
{
	const auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(Character::onKeyPressed, this);
	keyboardListener->onKeyReleased = [this](EventKeyboard::KeyCode, Event*) {
		moveHorizontal = 0;
		isFireAndStopMoving = false;
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	this->scheduleUpdate();
}

void Character::receiveDamage(const std::vector<Vec2>& damagedPoints)
{
	CCLOG("character get dmg");
}

float Character::findGroundDistanceAndNormal()
{
	const auto world = this->getScene()->getPhysicsWorld();
	if (!world)
		return 0;

	bool hit{ false };
	const auto rayLength = 100;

	PhysicsRayCastInfo raycastInfo;
	auto raycastCallback = [&hit, &raycastInfo, this](PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data) {
		if (info.shape->getBody() == physicsBody)
		{
			// self hit
			return true;
		}
		hit = true;
		raycastInfo = info;
		return false;
	};

	const Vec2 origin{ physicsBody->getPosition() };
	const Vec2 rayToGroundStart{ origin - Vec2::UNIT_Y * (radius + 1) };
	const Vec2 rayToGroundEnd{ Vec2{rayToGroundStart.x, rayToGroundStart.y - rayLength} };
	//const Vec2 rayToGroundEnd{ origin - Vec2::UNIT_Y * (radius + rayLength) };

	world->rayCast(raycastCallback, rayToGroundStart, rayToGroundEnd, nullptr);

	// Slope detect
	if (moveHorizontal)
	{
		const Vec2 rayToSlopeStart{ origin + Vec2::UNIT_X * (radius + 1) * moveHorizontal };
		const Vec2 rayToSlopeEnd{ Vec2{rayToSlopeStart.x, rayToGroundEnd.y} };
	}
	

	if (hit)
	{
		if (raycastInfo.normal.y > 0)
		{
			groundNormal = raycastInfo.normal;
			angle = 90 - MATH_RAD_TO_DEG(groundNormal.getAngle());
		}
		return raycastInfo.fraction * rayLength;
	}

	groundNormal = Vec2::UNIT_Y;
	angle = 90 - MATH_RAD_TO_DEG(groundNormal.getAngle());
	return rayLength;
}

// Mục đích của hàm này là xác định Character đã chạm đất hay chưa thôi
bool Character::onCollisionEnter(PhysicsContact& contact, PhysicsContactPreSolve& solve)
{
	PhysicsShape* self{};
	PhysicsShape* other{};
	if (!Helper::detectWhichCollider(contact, self, other, Character::COLLISION_CATEGORY))
		return true;

	if (self->getBody() != physicsBody)
		return true;

	const auto terrain = dynamic_cast<SpritePhysics*>(other->getBody()->getNode());
	if (!terrain)
		return true;

	isTouchGround = other;
	return true;
}

// Nếu Character rời khỏi shape của mặt đất mà nó đã đứng trước đó thì mới được tính là đã rời khỏi mặt đất
void Character::onCollisionExit(PhysicsContact& contact)
{
	PhysicsShape* self{};
	PhysicsShape* other{};
	if (!Helper::detectWhichCollider(contact, self, other, Character::COLLISION_CATEGORY))
		return;

	if (self->getBody() != physicsBody)
		return;

	const auto terrain = dynamic_cast<SpritePhysics*>(other->getBody()->getNode());
	if (!terrain)
		return;

	if (isTouchGround && other == isTouchGround)
		isTouchGround = nullptr;
}
