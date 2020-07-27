#include "Character.h"
#include "Helper.h"
#include <algorithm>
#include "SpritePhysics.h"
#include "SceneSetup.h"

namespace
{
	/// <summary>
	/// Clamp thành 1 trong 3 giá trị: -1 hoặc 0 hoặc 1
	/// </summary>
	/// <param name="value"></param>
	/// <returns></returns>
	int clamp1(int value)
	{
		if (value > 0)
			value = 1;
		else if (value < 0)
			value = -1;

		return value;
	}
}

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

	characterPresentation = std::make_unique<Presentation>();
	characterPresentation->init(this->name, spriteAnchor, spriteSize);
	this->addChild(characterPresentation->getSprite());

	characterMovement = std::make_unique<Movement>();
	characterMovement->init(this->getScene()->getPhysicsWorld(), physicsRadius);

	this->setPhysicsBody(characterMovement->getPhysicsBody());

	characterCannon = std::make_unique<Cannon>();

	return true;
}

void Character::update(float dt)
{
	switch (desiredState)
	{
		case Character::State::Idle:
		{
			characterPresentation->changeAnimation(State::Idle, true);
			break;
		}

		case Character::State::Move:
		{
			const auto moved = characterMovement->updatePosition(desiredMoveDirectionX, dt);
			if (moved)
			{
				doCharacterMoveCallback();
				characterPresentation->setRotation(characterMovement->getRotation(), desiredMoveDirectionX < 0);
				characterPresentation->changeAnimation(State::Move, true);
			}
			else
			{
				characterPresentation->changeAnimation(State::Idle, true);
			}
			break;
		}


		case Character::State::Attack:
			break;

		case Character::State::Aim:
		{
			const auto angle = characterCannon->aim(dt);
			doCharacterAimCallback();
			break;
		}

		case Character::State::Charge:
		{
			const auto power = characterCannon->charge(dt);
			doCharacterChargeCallback();
			break;
		}

		default:
			break;
	}

}

void Character::receiveDamage(const std::vector<Vec2>& damagedPoints)
{
	CCLOG("character get dmg");
}

#pragma region Controller

void Character::move(int directionX)
{
	if (directionX == 0)
		return;

	desiredState = State::Move;
	desiredMoveDirectionX = clamp1(directionX);
}

void Character::stopMove(int directionX)
{
	if (desiredMoveDirectionX == directionX || directionX == 0)
	{
		desiredState = State::Idle;
		desiredMoveDirectionX = 0;
	}
}

void Character::aim(int directionY)
{
	if (directionY == 0)
		return;

	desiredState = State::Aim;
	characterCannon->setAimDirection(clamp1(directionY));
}

void Character::stopAim(int directionY)
{
	if (characterCannon->getAimDirection() == directionY || directionY == 0)
	{
		desiredState = State::Idle;
		characterCannon->setAimDirection(0);
	}
}

void Character::startCharge()
{
	desiredState = State::Charge;
	desiredMoveDirectionX = 0;
}

void Character::stopChargeAndFire()
{
	desiredState = State::Attack;
	desiredMoveDirectionX = 0;
}

#pragma endregion

#pragma region Callbacks

void Character::doCharacterMoveCallback()
{
	const auto pos = convertToWorldSpaceAR(Vec2::ZERO);
	for (const auto& cb : characterMoveCallback)
	{
		if (cb)
			cb(pos);
	}
}

void Character::doCharacterAimCallback()
{
	for (const auto& cb : characterAimCallback)
	{
		if (cb)
			cb(characterCannon->getAimAngle());
	}
}

void Character::doCharacterChargeCallback()
{
	for (const auto& cb : characterChargeCallback)
	{
		if (cb)
			cb(characterCannon->getChargePower());
	}
}

void Character::setOnCharacterMoveCallback(std::function<void(Vec2 characterWPos)> callback)
{
	if (callback)
		characterMoveCallback.push_back(callback);
}

void Character::setOnCharacterAimCallback(const std::function<void(float aimAngle)> callback)
{
	if (callback)
		characterAimCallback.push_back(callback);
}

void Character::setOnCharacterChargeCallback(const std::function<void(float chargePower)> callback)
{
	if (callback)
		characterChargeCallback.push_back(callback);
}

#pragma endregion

