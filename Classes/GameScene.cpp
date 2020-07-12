#include "GameScene.h"
#include "Dino.h"

Scene* GameScene::createScene()
{
	return GameScene::create();
}

bool GameScene::init()
{
	if (!Scene::init())
		return false;

	// Init
	const auto size = this->getBoundingBox();

	// Dino
	Dino doux{ "DinoDoux" };
	this->addChild(doux.sprite);
	doux.sprite->setPosition(size.getMidX(), size.getMidY() + 100);
	doux.sprite->runAction(RepeatForever::create(Animate::create(doux.idleAnimation)));

	Dino mort{ "DinoTart" };
	this->addChild(mort.sprite);
	mort.sprite->setPosition(size.getMidX(), size.getMidY() - 100);
	mort.sprite->runAction(RepeatForever::create(Animate::create(mort.idleAnimation)));


	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyDown, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyUp, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
	//this->scheduleUpdate();

	return true;
}

void GameScene::update(float dt)
{
	const auto border = this->getBoundingBox();
	auto pos = character->getPosition();
	auto halfWidth = character->getBoundingBox().size.width / 2;

	if (keyPressed == KeyCode::KEY_LEFT_ARROW)
	{
		pos.x -= moveSpeed * dt;
	}
	else if (keyPressed == KeyCode::KEY_RIGHT_ARROW)
	{
		pos.x += moveSpeed * dt;
	}

	if (pos.x > (border.getMinX() + halfWidth) && pos.x < (border.getMaxX() - halfWidth))
		character->setPosition(pos);
}

void GameScene::onKeyDown(KeyCode key, Event* ev)
{
	if (key == KeyCode::KEY_LEFT_ARROW || key == KeyCode::KEY_RIGHT_ARROW)
		keyPressed = key;
}

void GameScene::onKeyUp(KeyCode key, Event* ev)
{
	if (key == keyPressed)
		keyPressed = KeyCode::KEY_NONE;
}