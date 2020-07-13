#include "GameTerrain.h"

Scene* GameTerrain::createScene()
{
	return GameTerrain::create();
}

bool GameTerrain::init()
{
	if (!Scene::init())
		return false;

	const auto winSize = Director::getInstance()->getWinSize();
	const Vec2 center = { winSize.width / 2, winSize.height / 2 };

	background = Sprite::create("Background.png");
	this->addChild(background);
	//background->setPosition(winSize.width / 2, winSize.height / 2);

	const auto clipper = ClippingNode::create();
	stencil = DrawNode::create();
	stencil->drawSolidCircle(center, 10, 0, 10, Color4F::BLACK);
	clipper->addChild(stencil);
	clipper->setStencil(stencil);
	clipper->setInverted(true);

	const auto picture = Sprite::create("HelloWorld.png");
	picture->setPosition(center);
	clipper->addChild(picture);

	this->addChild(clipper);

	// Mouse
	const auto mouseEv = EventListenerMouse::create();
	mouseEv->onMouseDown = [this](EventMouse* event) {
		stencil->drawSolidCircle(Vec2{ event->getCursorX(), event->getCursorY() }, 10, 0, 10, Color4F::BLACK);
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseEv, background);

	return true;
}
