#include <cmath>
#include "GunBound.h"
#include "ui/CocosGUI.h"

namespace
{
	// Helper Free Function.
	std::pair<Vec2, float> getUnitVector(Vec2 input)
	{
		const auto rad = std::atan2(input.y, input.x);
		input.x = std::cos(rad);
		input.y = std::sin(rad);
		return std::pair<Vec2, float> {input, rad* (180 / static_cast<float>(std::_Pi))};
	}
	std::pair<Vec2, float> getUnitVector(float x, float y)
	{
		const auto rad = std::atan2(y, x);
		Vec2 unit{};
		unit.x = std::cos(rad);
		unit.y = std::sin(rad);
		return std::pair<Vec2, float> {unit, rad* (180 / static_cast<float>(std::_Pi))};
	}
}

Scene* GunBound::createScene()
{
	return GunBound::create ();
}

bool GunBound::init ()
{
	if (!Scene::init ())
		return false;

	const auto winSize = Director::getInstance()->getWinSize();

	// UI
	forceLabel = Label::createWithSystemFont(std::to_string(force), "", 30);
	forceLabel->setPosition(winSize.width - 100, winSize.height - 50);
	this->addChild(forceLabel);

	directionLabel = Label::createWithSystemFont("", "", 30);
	directionLabel->setPosition(winSize.width - 100, winSize.height - 100);
	this->addChild(directionLabel);

	normalizeLabel = Label::createWithSystemFont("", "", 30);
	normalizeLabel->setPosition(winSize.width - 100, winSize.height - 150);
	this->addChild(normalizeLabel);

	const auto increaseForce = ui::Button::create ();
	increaseForce->setTitleFontSize(30);
	increaseForce->setTitleText("+");
	increaseForce->setPosition(Vec2{ winSize.width - 50, winSize.height - 50 });
	increaseForce->addTouchEventListener([&](Ref*, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED)
		{
			forceLabel->setString(std::to_string(++force));
		}
		});
	this->addChild(increaseForce);

	const auto decreaseForce = ui::Button::create ();
	decreaseForce->setTitleFontSize(30);
	decreaseForce->setTitleText("-");
	decreaseForce->setPosition(Vec2{ winSize.width - 150, winSize.height - 50 });
	decreaseForce->addTouchEventListener([&](Ref*, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED)
		{
			forceLabel->setString(std::to_string(--force));
		}
		});
	this->addChild(decreaseForce);

	// Cannon
	const auto cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile("Cannon/Cannon.plist", "Cannon/Cannon.png");

	cannon = Gun::createGun();
	cannon->setScale(0.3);
	this->addChild(cannon);

	// Cliping Mask
	// Stencil là hình sẽ cắt bớt đi hình chính
	const auto stencilNode = DrawNode::create ();
	stencilNode->drawSolidCircle(Vec2{ 100, 100 }, 30, 360, 12, Color4F::Color4F(0, 0, 0, 0));

	const auto clipNode = ClippingNode::create (stencilNode);

	// DrawNode là hình sẽ bị cắt bớt đi.
	const auto drawNode = Sprite::createWithSpriteFrameName("CannonWheel.png");
	drawNode->setColor(Color3B::GREEN);
	clipNode->addChild(drawNode);
	clipNode->setPosition(100, 100);
	clipNode->setInverted(true);
	this->addChild(clipNode);

	// Mouse Events
	const auto mouseListener = EventListenerMouse::create ();
	mouseListener->onMouseMove = [this](EventMouse* ev) {
		const auto value = getUnitVector(ev->getCursorX(), ev->getCursorY());
		direction = value.first;
		directionLabel->setString(std::to_string(value.second) + " degrees");
		normalizeLabel->setString(std::to_string(ev->getCursorX()) + ", " + std::to_string(ev->getCursorY()));

		cannon->setGunRotation(value.second);
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

	//this->scheduleUpdate();

	return true;
}

void GunBound::update(float dt)
{
	// Vẽ lên các chấm tròn biểu thị cho projectile của 1 viên cannon ball.
	// Số chấm cần vẽ là 10.
	const int numberOfDot{ 50 };
	Vec2 velocity = direction * force;
	Vec2 position = this->convertToNodeSpaceAR(cannon->getBarrelPos());
	int i{ };
	for (const auto dot : dots)
	{
		this->removeChild(dot);
	}
	dots.clear();

	do
	{
		velocity += acceleration * dt;
		position += velocity * dt;

		if (i++ % numberOfDot != 0)
			continue;

		// Draw Dot
		const auto dot = cocos2d::DrawNode::create ();
		dot->drawSolidCircle(position, 10, 360, 12, 1, 1, Color4F::WHITE);
		dots.push_back(dot);
		this->addChild(dot);
	} while (position.y > 0);
}