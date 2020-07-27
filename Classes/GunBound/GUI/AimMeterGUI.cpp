#include "AimMeterGUI.h"

bool AimMeterGUI::init()
{
    if(!Node::init())
    return false;

    circle = DrawNode::create();
    this->addChild(circle);
    circle->drawSolidCircle(Vec2::ZERO, 30, 0, 10, Color4F::Color4F(1, 1, 1, 0.5f));

    needle = DrawNode::create(5);
    this->addChild(needle);

    return true;
}

void AimMeterGUI::updateAngle(float needleAngle)
{
    needle->setRotation(-needleAngle);
}

void AimMeterGUI::updatePosition(Vec2& newWPos)
{
    const auto localPos = this->getParent()->convertToNodeSpace(newWPos);
    this->setPosition(localPos);
}
