#include "Hockey.h"

namespace
{
	// Helper Free Function
	Vec2 clamp(Vec2 pos, const Size& size, const Rect& range)
	{
		const auto halfWidth = size.width / 2;
		const auto halfHeight = size.height / 2;

		pos.x = MIN(MAX(pos.x, range.getMinX() + halfWidth), range.getMaxX() - halfWidth);
		pos.y = MIN(MAX(pos.y, range.getMinY() + halfHeight), range.getMaxY() - halfHeight);

		return pos;
	}
}

Scene* Hockey::createScene()
{
	const auto scene = Scene::create();
	scene->addChild(Hockey::create());
	return scene;
}

bool Hockey::init()
{
	if (!Layer::init())
		return false;

	// Add SpriteSheet
	const auto winSize = Director::getInstance()->getWinSize();
	const auto cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile("Hockey/Hockey.plist", "Hockey/Hockey.png");

	// Create Board
	board = Sprite::createWithSpriteFrameName("Board.png");
	this->addChild(board);
	board->setPosition(winSize.width / 2, winSize.height / 2);
	const float scaleRatio = (winSize.height / board->getContentSize().height);
	board->setScale(scaleRatio);
	const auto boardSize = board->getBoundingBox().size;

	// Create Players
	players.reserve(2);

	player1 = GameSprite::createGameSprite("Player.png");
	this->addChild(player1);
	player1->setScale(scaleRatio);
	player1->setPosition(winSize.width / 2, winSize.height - player1->getBoundingBox().size.height / 2);
	players.push_back(player1);

	player2 = GameSprite::createGameSprite("Player.png");
	this->addChild(player2);
	player2->setScale(scaleRatio);
	player2->setPosition(winSize.width / 2, player2->getBoundingBox().size.height / 2);
	players.push_back(player2);

	// Create Score Label
	player1_ScoreLabel = Label::createWithSystemFont("0", "", 30);
	this->addChild(player1_ScoreLabel);
	player1_ScoreLabel->setTextColor(Color4B::RED);
	player1_ScoreLabel->setPosition(winSize.width / 2 + boardSize.width / 2, winSize.height / 2 + player1_ScoreLabel->getContentSize().width);
	player1_ScoreLabel->setRotation(90);

	player2_ScoreLabel = Label::createWithSystemFont("0", "", 30);
	this->addChild(player2_ScoreLabel);
	player2_ScoreLabel->setTextColor(Color4B::BLUE);
	player2_ScoreLabel->setPosition(winSize.width / 2 + boardSize.width / 2, winSize.height / 2 - player2_ScoreLabel->getContentSize().width);
	player2_ScoreLabel->setRotation(90);

	// Touch Events
	const auto touchListener = EventListenerTouchAllAtOnce::create();
	touchListener->onTouchesBegan = CC_CALLBACK_2(Hockey::touchBegin, this);
	touchListener->onTouchesMoved = CC_CALLBACK_2(Hockey::touchMove, this);
	touchListener->onTouchesEnded = CC_CALLBACK_2(Hockey::touchEnd, this);
	touchListener->onTouchesCancelled = CC_CALLBACK_2(Hockey::touchEnd, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	return true;
}

void Hockey::touchBegin(const std::vector<Touch*>& touches, Event* ev)
{
	const auto touch = touches.at(0);
	const auto touchPos = touch->getLocation();

	for (const auto player : players)
	{
		if (player->getBoundingBox().containsPoint(touchPos))
		{
			player->setTouchFinger(touch);
		}
	}
}

void Hockey::touchMove(const std::vector<Touch*>& touches, Event* ev)
{
	const auto bounds = board->getBoundingBox();
	for (const auto player : players)
	{
		const auto finger = player->getTouchFinger();
		if (finger)
		{
			for (const auto touch : touches)
			{
				if (finger == touch)
				{
					auto touchPos = clamp(touch->getLocation(), player->getBoundingBox().size, bounds);
					player->setPosition(touchPos);
					break;
				}
			}
		}
	}
}

void Hockey::touchEnd(const std::vector<Touch*>& touches, Event* ev)
{
	for (const auto player : players)
	{
		const auto finger = player->getTouchFinger();
		if (finger)
		{
			for (const auto touch : touches)
			{
				if (finger == touch)
				{
					player->setTouchFinger(nullptr);
					break;
				}
			}
		}
	}
}
