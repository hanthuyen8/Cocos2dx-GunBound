#include "Character.h"

Character* Character::createInstance(std::string_view fileName, float radius)
{
	Character* instance = new Character();
	if (instance && instance->initWithFile(std::string{ fileName }))
	{
		instance->radius = radius;

		const auto body = PhysicsBody::create();
		const auto shape = PhysicsShapeCircle::create(radius, PhysicsMaterial::PhysicsMaterial(1, 0, 0));
		body->addShape(shape);
		body->setDynamic(true);
		body->setGravityEnable(false);
		instance->addComponent(body);
		instance->physicsBody = body;

		instance->autorelease();
		return instance;
	}

	CC_SAFE_DELETE(instance);
	return nullptr;
}

void Character::listenToKeyboardMovement()
{
	const auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(Character::onKeyPressed, this);
	keyboardListener->onKeyReleased = [this](EventKeyboard::KeyCode key, Event*) {
		moveDirection = 0;
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	this->scheduleUpdate();
}

void Character::setCollisionMask(int selfMask, int collideWith)
{
	physicsBody->setCategoryBitmask(selfMask);
	physicsBody->setCollisionBitmask(collideWith);
}

void Character::update(float dt)
{
	if (moveDirection != 0)
	{
		Vec2 curPos = this->getPosition();
		curPos.x += moveDirection * moveSpeed * dt;

		this->setPosition(curPos);

		/*if ((moveDirection < 0 && !isFacingToLeft) || (moveDirection > 0 && isFacingToLeft))
			this->setFlippedX(true);
		else
			this->setFlippedX(false);*/
	}
	checkGroundDistance(100 * dt);
}

void Character::onKeyPressed(EventKeyboard::KeyCode key, Event*)
{
	switch (key)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		moveDirection = -1;
		break;

	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		moveDirection = 1;
		break;

	default:

		moveDirection = 0;
		break;
	}
}

// Chỉ bắn ra 1 raycast từ tâm hướng xuống đất
// Lấy ra normal của điểm va chạm để biết hướng của mặt đất.
void Character::checkGroundDistance(float distance)
{
	const auto scene = this->getScene();
	if (scene)
	{
		Vec2 groundNormal{};
		int hit = 0;
		auto raycastCallback = [&groundNormal, &hit, this](PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data) {
			if (info.shape->getBody() == physicsBody)
			{
				return true;
			}
			groundNormal = info.normal;
			hit = 1;
			return false;
		};

		const Vec2 begin{ physicsBody->getPosition() + Vec2::UNIT_Y * radius };
		const Vec2 end{ begin + (Vec2::UNIT_Y * distance) };

		scene->getPhysicsWorld()->rayCast(raycastCallback, begin, end, nullptr);

		if (hit == 1)
		{
			// Chạm đất
			// Lấy ra angle của mặt đất rồi gán nó cho Character
			/*const float angle = groundNormal.getAngle();
			this->setRotation(MATH_RAD_TO_DEG(angle));*/
		}
		else
		{
			// Trên không
			Vec2 curPos = this->getPosition();
			curPos.y -= distance;
			this->setPosition(curPos);
		}
	}
}
