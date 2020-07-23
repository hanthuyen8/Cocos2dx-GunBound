#include "Character.h"
#include "Helper.h"
#include <algorithm>
#include "SpritePhysics.h"
#include "SceneSetup.h"

const int FALL_SPEED = 981;
const int MAX_SLOPE_ANGLE = 60;

Character* Character::create(std::string_view characterName, const Vec2& spriteAnchor, const Size& spriteSize, float physicsRadius)
{
	Character* instance = new(std::nothrow) Character();
	if (instance && instance->init(characterName, spriteAnchor, spriteSize, physicsRadius))
	{
		instance->autorelease();
		return instance;
	}

	CC_SAFE_DELETE(instance);
	return nullptr;
}

bool Character::init(std::string_view characterName, const Vec2& spriteAnchor, const Size& spriteSize, float physicsRadius)
{
	if (!Node::init())
		return false;

	this->name = characterName;
	this->radius = physicsRadius;

	sprite = Sprite::create();
	CC_ASSERT(sprite);
	sprite->setContentSize(spriteSize);
	sprite->setAnchorPoint(spriteAnchor);
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

	cannon->setLocalZOrder(0);
	sprite->setLocalZOrder(1);
	changeAnimation(Character::AnimState::Idle, true);

	return true;
}

void Character::update(float dt)
{
	const auto groundDistance = findGroundDistanceAndNormal();
	sprite->setRotation(clampf(angle, -90, 90));
	const auto velocity = physicsBody->getVelocity();

	if (moveHorizontal != 0)
	{
		// Khi Character có lệnh di chuyển

		// Gravity nghiêng theo mặt đất
		auto gravity = FALL_SPEED * -1 * groundNormal;
		physicsBody->applyForce(gravity);

		// clamp velocity.x không cho vượt quá moveSpeed
		auto force_x = std::abs(velocity.x);
		if (sprite->isFlippedX() != (moveHorizontal < 0))
		{
			// Phát hiện chuyển hướng
			// Quất ngay 1 lực hướng ngược lại để chống trượt dài theo velocity cũ
			force_x = velocity.x * -1 / dt;
			sprite->setFlippedX(moveHorizontal < 0);
		}
		else
		{
			// Không phát hiện chuyển hướng
			force_x = (moveSpeed - force_x) * moveHorizontal;
		}
		physicsBody->applyForce(Vec2{ force_x ,0 });
		doPlayerMovementCallback();
		changeAnimation(Character::AnimState::Walk, true);
	}
	else
	{
		// Khi Character không có lệnh di chuyển -> đứng yên

		if (!isTouchGround && groundDistance > 2)
		{
			// Gravity hút xuống đất
			float y = -FALL_SPEED;

			// hãm lại x
			float x = -std::abs(velocity.x);

			physicsBody->applyForce(Vec2{ x, y });
			doPlayerMovementCallback();
			changeAnimation(Character::AnimState::Walk, true);
		}
		else
		{
			physicsBody->setVelocity(Vec2::ZERO);
			changeAnimation(Character::AnimState::Idle, true);
		}
	}
}

void Character::receiveDamage(const std::vector<Vec2>& damagedPoints)
{
	CCLOG("character get dmg");
}

#pragma region Key Board Movement

void Character::onKeyPressed(EventKeyboard::KeyCode key, Event*)
{
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

void Character::onKeyReleased(EventKeyboard::KeyCode key, Event*)
{
	switch (key)
	{
		case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			if (moveHorizontal == -1)
				moveHorizontal = 0;
			break;

		case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			if (moveHorizontal == 1)
				moveHorizontal = 0;
			break;

		default:
			break;
	}
}

void Character::listenToKeyboardMovement()
{
	const auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(Character::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(Character::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	this->scheduleUpdate();
}

void Character::setOnCharacterMovementCallback(std::function<void(Vec2 characterWPos)> callback)
{
	if (callback)
		characterMovementCallback.push_back(callback);
}

void Character::doPlayerMovementCallback()
{
	const auto pos = convertToWorldSpaceAR(Vec2::ZERO);
	for (const auto cb : characterMovementCallback)
	{
		if (cb)
			cb(pos);
	}
}

void Character::changeAnimation(AnimState newState, bool loop)
{
	if (currentAnim == newState)
		return;

	currentAnim = newState;
	std::string animName{};
	switch (newState)
	{
		case Character::AnimState::Idle:
			animName = name + AnimSuffix::IDLE;
			break;
		case Character::AnimState::Walk:
			animName = name + AnimSuffix::WALK;
			break;
		case Character::AnimState::Attack:
			animName = name + AnimSuffix::ATTACK;
			break;
	}
	if (animName.empty())
		return;

	Animation* anim{};
	const auto cache = AnimationCache::getInstance();
	anim = AnimationCache::getInstance()->getAnimation(animName);

	if (anim)
	{
		sprite->stopAllActions();
		const auto animateAction = Animate::create(anim);
		if (loop)
			sprite->runAction(RepeatForever::create(animateAction));
		else
			sprite->runAction(animateAction);
	}
}

#pragma endregion

#pragma region Collision And Find Paths

bool Character::raycastHit(PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data)
{
	if (info.shape->getBody() == physicsBody)
	{
		// self groundDetected
		return true;
	}
	*(static_cast<PhysicsRayCastInfo*>(data)) = info;
	return false;
}

float Character::findGroundDistanceAndNormal()
{
	const auto world = this->getScene()->getPhysicsWorld();
	if (!world)
		return 0;

	const auto rayLength = 100;

	PhysicsRayCastInfo groundInfo{};
	PhysicsRayCastInfo slopeInfo{};

	const Vec2 origin{ physicsBody->getPosition() };
	const auto func = CC_CALLBACK_3(Character::raycastHit, this);

	/*
	Cách hoạt động:
	- Bắn 2 raycast để lấy ground & slope.
	- Kẻ 1 đường nối 2 điểm contact đó, đường đó được xem như là ground mới.
	- Lấy groundNormal và fraction như sau:
		- Nếu slope fraction <= 0 thì lấy normal và fraction của ground;
		- Nếu slope normal == groundNormal thì lấy normal và fraction của ground;
		- Nếu slope normal != groundNormal thì lấy normal mới và trung bình cộng của 2 fraction.
	*/

	if (moveHorizontal)
	{
		// Raycast to detect Slope
		const Vec2 rayToSlopeStart{ origin + Vec2::UNIT_X * (radius + 1) * moveHorizontal };
		const Vec2 rayToSlopeEnd{ rayToSlopeStart - groundNormal * rayLength };
		world->rayCast(func, rayToSlopeStart, rayToSlopeEnd, &slopeInfo);
	}

	// Raycast to detect Ground
	const Vec2 rayToGroundStart{ origin - Vec2::UNIT_Y * (radius + 1) };
	const Vec2 rayToGroundEnd{ Vec2{rayToGroundStart.x, rayToGroundStart.y - rayLength} };
	//const Vec2 rayToGroundEnd{ origin - Vec2::UNIT_Y * (radius + rayLength) };
	world->rayCast(func, rayToGroundStart, rayToGroundEnd, &groundInfo);

	/*
		- Nếu slope fraction <= 0 thì lấy normal và fraction của ground;
		- Nếu slope normal == groundNormal thì lấy normal và fraction của ground;
		- Nếu slope normal != groundNormal thì lấy normal mới và trung bình cộng của 2 fraction.
	*/
	Vec2 resultNormal{};
	float resultFraction{};

	const auto setResult = [&resultNormal, &resultFraction, rayLength](PhysicsRayCastInfo& info) {
		resultNormal = info.normal;
		resultFraction = info.fraction * rayLength;
	};

	const bool groundDetected{ groundInfo.shape != nullptr && groundInfo.normal.y > 0 };
	const bool slopeDetected{ slopeInfo.shape != nullptr && slopeInfo.normal.y > 0 };

	if (!groundDetected)
	{
		// Ko groundDetected trúng Ground -> Lấy y
		resultNormal = Vec2::UNIT_Y;
		resultFraction = rayLength;
	}
	else
	{
		if (!slopeDetected || slopeInfo.normal == groundInfo.normal)
		{
			// Nếu không bắn trúng Slope
			// Hoặc bắn trúng nhưng Slope và Ground cùng phương
			setResult(groundInfo);
		}
		else
		{
			// Slope và Ground khác phương
			// Nhiều trường hợp nếu vẫn để y thì Character sẽ bị stuck ở đáy hoặc ở đỉnh của polygon
			// Bỏ luôn y ở trường hợp này cho khỏe
			groundNormal = Vec2::ZERO;
			return 0;
		}
	}

	groundNormal = resultNormal;
	angle = 90 - MATH_RAD_TO_DEG(resultNormal.getAngle());
	return resultFraction;
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

#pragma endregion