#include "Cannon.h"
#include "GunBound/GunBoundScene.h"

const float MIN_ANGLE = 0;
const float MAX_ANGLE = 180;
const float AIM_SPEED = 200;
const float CHARGE_SPEED = 70;

Cannon* Cannon::createInstance()
{
	Cannon* cannon = new Cannon();
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

	aimOnAir = DrawNode::create();
	this->addChild(aimOnAir);

	aimMeter = DrawNode::create();
	this->addChild(aimMeter);

	chargeMeter = DrawNode::create();
	this->addChild(chargeMeter);

	const auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(Cannon::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(Cannon::stopAiming, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	return true;
}

void Cannon::update(float dt)
{
	if (isAiming)
	{
		angle = clampf(angle + isAiming * AIM_SPEED * dt, MIN_ANGLE, MAX_ANGLE);
		const auto rad{ CC_DEGREES_TO_RADIANS(angle) };
		aimOnAir->clear();
		aimOnAir->setLineWidth(5);
		aimOnAir->drawLine(Vec2::ZERO, Vec2::ZERO + Vec2{ std::cos(rad), std::sin(rad) } *30, Color4F::GREEN);
	}

	if (isCharging)
	{
		chargeMeter->clear();
		chargeMeter->setLineWidth(20);
		chargeMeter->drawLine(Vec2{ -50, -50 }, Vec2{ 50, -50 }, Color4F::WHITE);

		isCharging = clampf(isCharging + CHARGE_SPEED * dt, 0, 100);
		log("%f", isCharging);
		const auto charge = MathUtil::lerp(-50, 50, isCharging / 100);
		chargeMeter->drawLine(Vec2{ -50, -50 }, Vec2{ charge, -50 }, Color4F::GREEN);
	}
}

void Cannon::drawPath(float dt)
{
	this->clear();

	// Vẽ lên các chấm tròn biểu thị cho projectile của 1 viên cannon ball.
	// Số chấm cần vẽ là 10.
	const int numberOfDot{ 50 };
	Vec2 velocity{ fireDirection * force };
	Vec2 start{ this->getPosition() };
	Vec2 acceleration{ GunBoundScene::getAcceleration() };
	int i{ };
	this->drawCircle(Vec2::ZERO, 10, 10, 10, false, Color4F::WHITE);
	return;

	do
	{
		velocity += acceleration * dt;
		start += velocity * dt;

		if (i++ % numberOfDot != 0)
			continue;

		// Draw Dot
		this->drawSolidCircle(start, 10, 360, 10, 1, 1, Color4F::WHITE);
	} while (start.y > 0);
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

void Cannon::stopAiming(EventKeyboard::KeyCode, Event*)
{
	isAiming = false;
	this->unscheduleUpdate();
	const auto fadeOut = FadeOut::create(2);
	fadeOut->setTag(0);
	this->runAction(fadeOut);
}
