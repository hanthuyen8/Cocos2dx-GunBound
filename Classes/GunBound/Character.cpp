#include "Character.h"
#include "Helper.h"
#include <algorithm>
#include "SpritePhysics.h"

const int FALL_SPEED = 981;

Character* Character::create(std::string_view fileName, float radius)
{
	Character* instance = new(std::nothrow) Character();
	if (instance && instance->init(fileName, radius))
	{
		instance->autorelease();
		return instance;
	}

	CC_SAFE_DELETE(instance);
	return nullptr;
}

bool Character::init(std::string_view fileName, float radius)
{
	if (!Sprite::initWithFile(std::string{ fileName }))
		return false;

	this->radius = radius;

	physicsBody = PhysicsBody::create();
	physicsBody->addShape(PhysicsShapeCircle::create(radius, PhysicsMaterial::PhysicsMaterial(0, 0, 0)));
	physicsBody->setDynamic(true);
	physicsBody->setGravityEnable(true);
	physicsBody->setRotationEnable(true);

	physicsBody->setCategoryBitmask(COLLISION_CATEGORY);
	physicsBody->setContactTestBitmask(COLLISION_WITH);

	const auto collisionListener = EventListenerPhysicsContact::create();
	collisionListener->onContactPreSolve = CC_CALLBACK_2(Character::onCollisionEnter, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(collisionListener, this);

	this->setPhysicsBody(physicsBody);

	cannon = Cannon::create(radius);
	RETURN_FALSE_IF_NULL_PTR(cannon, "Character cannon");

	cannon->setPosition(this->convertToNodeSpace(Vec2::ZERO));
	this->addChild(cannon);

	return true;
}

void Character::update(float dt)
{
	if (!isFireAndStopMoving)
	{
		if (moveHorizontal != 0)
		{
			Vec2 velocity = {};
			this->setFlippedX(moveHorizontal < 0);

			// Để di chuyển được tới phía trước thì phải sinh ra 1 lực y lớn hơn Gravity và tạo thành 1 vector(x,y) sao cho song song với mặt đất
			// Vậy đầu tiên phải lấy ra được hướng song song với mặt đất, chính là góc của Character
			velocity.x = moveHorizontal * moveSpeed * dt;
			physicsBody->applyImpulse(velocity);
			physicsBody->applyImpulse(physicsBody->getWorld()->getGravity() * -dt);
			this->setRotation(angle);
			Helper::logVec2(physicsBody->getVelocity());
		}
		else
		{
			const auto distanceVsGround = checkGround();
			if (distanceVsGround >= 0 && distanceVsGround < 2)
				physicsBody->setVelocity(Vec2::ZERO);
		}
	}
	else
	{
		// Write here
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

float Character::checkGround()
{
	const auto world = this->getScene()->getPhysicsWorld();
	if (!world)
		return 0;

	bool hit{ false };
	const auto gravityForce = -world->getGravity().y;

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
	const Vec2 startPoint{ origin - groundNormal * (radius) };
	const Vec2 endPoint{ origin - groundNormal * (radius + gravityForce) };

	world->rayCast(raycastCallback, startPoint, endPoint, nullptr);

	if (hit)
	{
		return raycastInfo.fraction * gravityForce;
	}
	return -gravityForce;
}

bool Character::onCollisionEnter(PhysicsContact& contact, PhysicsContactPreSolve& solve)
{
	PhysicsShape* self{};
	PhysicsShape* other{};
	if (!Helper::detectWhichCollider(contact, self, other, Character::COLLISION_CATEGORY))
		return true;

	const SpritePhysics* terrain = dynamic_cast<SpritePhysics*>(other->getBody()->getNode());
	if (!terrain)
		return true;

	const auto characterPos = physicsBody->getPosition();
	const auto collisionPos = contact.getContactData()->points[0];

	if (characterPos.y > collisionPos.y)
	{
		// Đã chạm đất. Lấy hướng của mặt đất và xoay Character
		// Mình sẽ map như thế này
		// Nếu mặt đất nằm ngang thì normalVec của nó sẽ là hướng lên trời (angle == 90)
		// Lúc này thì Character sẽ phải có angle == 0 (vì hình nhân vật luôn đứng thẳng mà hình mặt đất lại luôn nằm ngang)
		// Cho nên sẽ có 1 bước map từ 90deg của mặt đất sang 0deg của Character.
		// Lưu ý: Hàm setRotation xoay theo chiều kim đồng hồ.
		groundNormal = (characterPos - collisionPos).getNormalized();
		angle = 90 - MATH_RAD_TO_DEG(groundNormal.getAngle());
	}

	return true;
}
