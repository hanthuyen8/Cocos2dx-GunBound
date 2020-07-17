#include "Cannon.h"
#include "GunBound/GunBoundScene.h"
#include "Helper.h";

const float MIN_ANGLE = 0;
const float MAX_ANGLE = 180;
const float AIM_SPEED = 200;
const float CHARGE_SPEED = 70;

Cannon* Cannon::create()
{
	Cannon* cannon = new(std::nothrow) Cannon();
	if (cannon && cannon->init())
	{
		cannon->autorelease();
		return cannon;
	}

	CC_SAFE_DELETE(cannon);
	return nullptr;
}

bool Cannon::init()
{
	if (!DrawNode::init())
		return false;

	this->drawSolidCircle(Vec2::ZERO, 30, 0, 10, Color4F::Color4F(1, 1, 1, 1));
	this->setCascadeOpacityEnabled(true);
	this->setOpacity(0);

	ammoPath = DrawNode::create();
	this->addChild(ammoPath);

	aimMeter = DrawNode::create();
	this->addChild(aimMeter);

	chargeMeter = DrawNode::create();
	this->addChild(chargeMeter);

	ammo = Ammo::create();
	RETURN_FALSE_IF_NULL_PTR(ammo, "Cannon ammo");
	this->addChild(ammo);

	const auto keyboardListener = EventListenerKeyboard::create();
	if (!keyboardListener)
		return false;

	keyboardListener->onKeyPressed = CC_CALLBACK_2(Cannon::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(Cannon::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	return true;
}

void Cannon::update(float dt)
{
	if (isAiming)
	{
		angle = clampf(angle + isAiming * AIM_SPEED * dt, MIN_ANGLE, MAX_ANGLE);
		const float rad{ CC_DEGREES_TO_RADIANS(angle) };
		aimMeter->clear();
		aimMeter->setLineWidth(5);
		aimMeter->drawLine(Vec2::ZERO, Vec2::ZERO + Vec2{ std::cos(rad), std::sin(rad) } *30, Color4F::GREEN);
	}

	if (isCharging)
	{
		chargeMeter->clear();
		chargeMeter->setLineWidth(20);
		chargeMeter->drawLine(Vec2{ -50, -50 }, Vec2{ 50, -50 }, Color4F::WHITE);

		isCharging = clampf(isCharging + CHARGE_SPEED * dt, 0, 100);
		const auto charge = MathUtil::lerp(-50, 50, isCharging / 100);
		chargeMeter->drawLine(Vec2{ -50, -50 }, Vec2{ charge, -50 }, Color4F::GREEN);

		const auto path = ammo->drawPath(angle, isCharging, dt);
		ammoPath->clear();
   		for (const auto& point : path)
		{
			ammoPath->drawDot(point, 10, Color4F::RED);
		}
	}
}

void Cannon::fire()
{
	ammo->fire(angle, isCharging);
}

void Cannon::onKeyPressed(EventKeyboard::KeyCode key, Event*)
{
	isAiming = false;
	isCharging = false;

	switch (key)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
		isAiming = 1;
		break;

	case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		isAiming = -1;
		break;

	case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
		isCharging = 1;
		break;

	default:
		break;
	}

	if (isAiming || isCharging)
	{
		this->scheduleUpdate();
		this->stopActionByTag(0);
		this->setOpacity(255);
	}
}

void Cannon::onKeyReleased(EventKeyboard::KeyCode key, Event*)
{
	if (key == cocos2d::EventKeyboard::KeyCode::KEY_SPACE)
	{
		fire();
	}

	isCharging = false;
	isAiming = false;
	ammoPath->clear();
	this->unscheduleUpdate();
	const auto fadeOut = FadeOut::create(2);
	fadeOut->setTag(0);
	this->runAction(fadeOut);
}
