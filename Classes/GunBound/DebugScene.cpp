#include "DebugScene.h"

Scene* DebugScene::createScene()
{
	return DebugScene::create();
}

bool DebugScene::init()
{
	if (!Scene::initWithPhysics())
		return false;

	sprite = Sprite::create("GunBound/Bullet.png");
	sample = Sprite::create("GunBound/Bullet.png");
	this->addChild(sprite);
	this->addChild(sample);

	physicsBody = PhysicsBody::create();
	physicsBody->setMass(0);
	physicsBody->setDynamic(false);
	physicsBody->setGravityEnable(false);
	sprite->addComponent(physicsBody);

	this->scheduleUpdate();

	const auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode key, Event*) {

		if (key == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
			moveHorizontal = -1;
		else if (key == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
			moveHorizontal = 1;
	};

	keyboardListener->onKeyReleased = [this](EventKeyboard::KeyCode, Event*) {
		moveHorizontal = 0;
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	sprite->setPosition(300, 300);
	sample->setPosition(300, 200);
	this->scheduleUpdate();

	return true;
}

void DebugScene::update(float dt)
{
	if (moveHorizontal)
	{
		auto vel = physicsBody->getVelocity();
		if (moveHorizontal > 0)
			vel.x = 450;
		else
			vel.x = -450;

		physicsBody->setVelocity(vel);
		log("%f", vel.x);

		auto sampleVel = sample->getPosition();
		if (moveHorizontal > 0)
			sampleVel.x += 450 * dt;
		else
			sampleVel.x += -450 * dt;
		sample->setPosition(sampleVel);
	}
	else
	{
		physicsBody->setVelocity(Vec2::ZERO);
	}
}


