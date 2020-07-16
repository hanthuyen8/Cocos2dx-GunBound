#include "Character.h"
#include "Helper.h"
#include <algorithm>

Character* Character::createInstance(std::string_view fileName, float radius)
{
	Character* instance = new Character();
	if (instance && instance->initWithFile(std::string{ fileName }, radius))
	{
		instance->autorelease();
		return instance;
	}

	CC_SAFE_DELETE(instance);
	return nullptr;
}

bool Character::initWithFile(const std::string& filename, float radius)
{
	if (!Sprite::initWithFile(filename))
		return false;

	this->radius = radius;
	moveSpeed = 6000;

	physicsBody = PhysicsBody::create();
	physicsBody->addShape(PhysicsShapeCircle::create(radius, PhysicsMaterial::PhysicsMaterial(1, 0, 0)));
	physicsBody->setDynamic(true);
	physicsBody->setGravityEnable(false);
	this->addComponent(physicsBody);

	cannon = Cannon::createInstance();
	//cannon->setPosition(this->convertToNodeSpace(Vec2{ 0, radius * 2 }));
	cannon->setPosition(this->convertToNodeSpace(Vec2::ZERO));
	this->addChild(cannon);

	return true;
}

void Character::setCollisionMask(int selfMask, int collideWith)
{
	physicsBody->setCategoryBitmask(selfMask);
	//physicsBody->setCollisionBitmask(collideWith);
	physicsBody->setContactTestBitmask(collideWith);
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

void Character::update(float dt)
{
	Vec2 velocity = {};

	if (!isFireAndStopMoving)
	{
		if (moveHorizontal != 0)
		{
			velocity.x = moveHorizontal * moveSpeed * dt;
			physicsBody->setVelocity(Vec2{ moveHorizontal * moveSpeed * dt, 0 });

			this->setFlippedX(moveHorizontal < 0);
		}
		velocity.y = checkGravity() * fallSpeed * dt;
		if (std::fabs(velocity.y) < 0.1f)
			velocity.y = 0;
	}
	else
	{
	}

	physicsBody->setVelocity(velocity);
}

// Chỉ bắn ra 1 raycast từ tâm hướng xuống đất
// Lấy ra normal của điểm va chạm để biết hướng của mặt đất.
/* Trả về: factor để x với speed
a.  0: đứng yên
b. -1 -> 0: rơi xuống
c.  0 -> 1: bay lên
*/

float Character::checkGravity()
{
	const auto scene = this->getScene();
	if (!scene)
		return 0;

	bool hit{ false };
	std::vector<PhysicsRayCastInfo> raycastInfos;
	auto raycastCallback = [&hit, &raycastInfos, this](PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data) {
		if (info.shape->getBody() == physicsBody)
		{
			// self hit
			return true;
		}
		hit = true;
		raycastInfos.push_back(info);
		return false;
	};

	const Vec2 rayLength{ Vec2::UNIT_Y * 1 };
	const Vec2 bottomPoint{ physicsBody->getPosition() - Vec2::UNIT_Y * (radius + 1) };
	const Vec2 lastNormalPoint{ physicsBody->getPosition() - groundNormal * (radius + 1) };

	const auto world = scene->getPhysicsWorld();

	// Bắn 2 phát, phát nào có fraction nhỏ hơn thì lấy

	if (!groundNormal.isZero())
		world->rayCast(raycastCallback, lastNormalPoint, lastNormalPoint - rayLength, nullptr);

	world->rayCast(raycastCallback, bottomPoint, bottomPoint - rayLength, nullptr);

	if (hit)
	{
		PhysicsRayCastInfo* bestRay = &*std::min_element(raycastInfos.begin(), raycastInfos.end(),
			[](PhysicsRayCastInfo& ray1, PhysicsRayCastInfo& ray2) {
				return ray1.fraction < ray2.fraction;
			});

		// Chạm đất
		if (!currentCollider)
			currentCollider = bestRay->shape;

		else if (currentCollider != bestRay->shape)
		{
			currentCollider = bestRay->shape;

			// Lấy ra angle của mặt đất rồi gán nó cho Character
			// Theo nghiên cứu thì normal của mặt đất chỉ nằm trong khoảng quadrent 1 và 2:
			// Tức là trong khoảng [(-1, 0) -> (1,0)] (y luôn luôn dương)
			// Đổi sang độ là (xoay ngược chiều kim) : 0 - 180 hoặc 0pi - 1pi

			const auto& normal = bestRay->normal;
			if (normal.y < 0)
			{
				this->setRotation(0);
				groundNormal = Vec2{ 0,1 };
			}
			else
			{
				// Mình sẽ map như thế này
				// Nếu mặt đất nằm ngang thì normalVec của nó sẽ là hướng lên trời (angle == 90)
				// Lúc này thì Character sẽ phải có angle == 0 (vì hình nhân vật luôn đứng thẳng mà hình mặt đất lại luôn nằm ngang)
				// Cho nên sẽ có 1 bước map từ 90deg của mặt đất sang 0deg của Character.
				// Lưu ý: Hàm setRotation xoay theo chiều kim đồng hồ.
				const float baseAngle = 90;
				const auto xAxisAngle = MATH_RAD_TO_DEG(normal.getAngle());
				const auto diffVsBaseAngle = baseAngle - xAxisAngle;
				this->setRotation(diffVsBaseAngle);
				groundNormal = normal;
			}
		}
		return -bestRay->fraction;
	}
	else
	{
		return -1;
	}
	return 0;
}
