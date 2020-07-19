#include "DebugScene.h"

Scene* DebugScene::createScene()
{
	return DebugScene::create();
}

bool DebugScene::init()
{
	if (!Scene::initWithPhysics())
		return false;

	const auto sprite = Sprite::create("GunBound/Bullet.png");
	this->addChild(sprite);

	physicsBody = PhysicsBody::create();
	physicsBody->setDynamic(false);
	physicsBody->setGravityEnable(false);
	sprite->addComponent(physicsBody);

	physicsBody->setVelocity(Vec2{ 100,100 });
	this->scheduleUpdate();

	return true;
}

void DebugScene::update(float dt)
{
	physicsBody->applyForce(Vec2{ 0, -500 } *dt);
}


