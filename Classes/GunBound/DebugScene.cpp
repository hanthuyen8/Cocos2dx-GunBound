#include "DebugScene.h"

Scene* DebugScene::createScene()
{
	return DebugScene::create();
}

bool DebugScene::init()
{
	if (!Scene::init())
		return false;

	lblAngleDeg = Label::createWithSystemFont("", "", 20);
	lblAngleDeg->setAnchorPoint(Vec2{ 0,0 });
	lblAngleDeg->setPosition(Vec2{ 0, 400 });
	this->addChild(lblAngleDeg);

	lblAngleRad = Label::createWithSystemFont("", "", 20);
	lblAngleRad->setAnchorPoint(Vec2{ 0,0 });
	lblAngleRad->setPosition(Vec2{ 0, 350 });
	this->addChild(lblAngleRad);

	lblNormal = Label::createWithSystemFont("", "", 20);
	lblNormal->setAnchorPoint(Vec2{ 0,0 });
	lblNormal->setPosition(Vec2{ 0, 300 });
	this->addChild(lblNormal);

	lblXAxisAngle = Label::createWithSystemFont("", "", 20);
	lblXAxisAngle->setAnchorPoint(Vec2{ 0,0 });
	lblXAxisAngle->setPosition(Vec2{ 0, 200 });
	this->addChild(lblXAxisAngle);

	drawNode = DrawNode::create();
	drawNode->setPosition(Vec2{ 300, 300 });
	this->addChild(drawNode);

	const auto mouseListener = EventListenerMouse::create();
	mouseListener->onMouseDown = [this](EventMouse* ev) {
		const auto rad = MATH_DEG_TO_RAD(angle);
		const auto normal = Vec2{ std::cos(rad), std::sin(rad) };
		drawNode->clear();
		drawNode->drawLine(drawNode->getPosition(), drawNode->getPosition() + normal * 100, Color4F::GREEN);

		lblAngleDeg->setString("Angle (Deg): " + std::to_string(angle));
		lblAngleRad->setString("Angle (Rad): " + std::to_string(rad));
		lblNormal->setString("Normal: " + std::to_string(normal.x) + ", " + std::to_string(normal.y));
		lblXAxisAngle->setString("Normal to angle X: " + std::to_string(normal.getAngle()));

		angle += 30;
		if (angle == 360)
			angle = 0;
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

	return true;
}
