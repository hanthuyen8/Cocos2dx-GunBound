#include "ChargeMeterGUI.h"

bool ChargeMeterGUI::init()
{
	if (!DrawNode::init())
		return false;

	this->drawSolidCircle(Vec2::ZERO, 30, 0, 10, Color4F::Color4F(1, 1, 1, 0.5f));
	needle = DrawNode::create(5);
	this->addChild(needle);
	needle->setPosition(this->getBoundingBox().getMidX(), this->getBoundingBox().getMidY());

	return true;
}

void ChargeMeterGUI::show(Vec2 atWPos)
{

}

void ChargeMeterGUI::updateAngle(float angle)
{

}

void ChargeMeterGUI::setFlipX(bool left)
{

}

void ChargeMeterGUI::hide()
{
}
