#include "DebugScene.h"

Scene* DebugScene::createScene()
{
	return DebugScene::create ();
}

bool DebugScene::init ()
{
	if (!Scene::initWithPhysics())
		return false;

	sprite = Sprite::create ("GunBound/Bullet.png");
	sample = Sprite::create ("GunBound/Bullet.png");
	this->addChild(sprite);
	sprite->setPosition(400, 400);
	sprite->addChild(sample);

	return true;
}

void DebugScene::update(float dt)
{
}


