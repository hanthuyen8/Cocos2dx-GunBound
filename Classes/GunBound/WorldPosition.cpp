#include "WorldPosition.h"

Vec2 WorldPosition::getWPosition()
{
	return parent->convertToWorldSpaceAR(this->getPosition());
}

void WorldPosition::setWPosition(Vec2 wpos)
{
	this->setPosition(parent->convertToNodeSpaceAR(wpos));
}
