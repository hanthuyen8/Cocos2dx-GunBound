#include <cmath>
#include "Gun.h"

namespace
{
	// Free Functions
	Vec2 rotatePointAroundOrigin(const Vec2 point, const float angle)
	{
		// Công thức counter-clockwise rotation:
		// x2 = cosA * x1 − sinA * y1
		// y2 = sinA * x1 + cosA * y1

		// Công thức clockwise rotation:
		// x2 = cosA * x1 + sinA * y1
		// y2 = -sinA * x1 + cosA * y1

		const auto rad{ CC_DEGREES_TO_RADIANS(-angle) };
		const float cosA = std::cos(rad);
		const float sinA = std::sin(rad);
		Vec2 output{ 0,0 };
		output.x = cosA * point.x - sinA * point.y;
		output.y = sinA * point.x + cosA * point.y;
		return output;
	}

	Vec2 rotateOffsetArountPoint(Vec2 point, const Vec2 offset, const float angle)
	{
		const auto rad{ CC_DEGREES_TO_RADIANS(angle) };
		const float cosA = std::cos(rad);
		const float sinA = std::sin(rad);

		// Translate Point back to Origin
		point -= offset;

		Vec2 output{};
		output.x = cosA * point.x - sinA * point.y;
		output.y = sinA * point.x + cosA * point.y;

		// Translate Point back to Offet
		point += output + offset;
		return point;
	}
}

// PUBLIC STATIC METHODS

Gun* Gun::createGun()
{
	auto gun = new Gun();
	if (gun)
	{
		gun->autorelease();
		return gun;
	}

	CC_SAFE_DELETE(gun);
	return nullptr;
}

// PUBLIC METHODS

Gun::Gun()
{
	this->setAnchorPoint(Vec2{ 0,0 });
	carSprite = Sprite::createWithSpriteFrameName("CannonWheel.png");
	gunSprite = Sprite::createWithSpriteFrameName("CannonGun.png");

	this->addChild(gunSprite);
	this->addChild(carSprite);

	carSprite->setAnchorPoint(Vec2{ 0,0 });
	gunSprite->setPosition(carSprite->getBoundingBox().getMidX(), carSprite->getBoundingBox().getMidY());
}

Vec2 Gun::getBarrelPos()
{
	const auto parent = this->getParent();
	if (parent)
	{
		return parent->convertToWorldSpaceAR(gunSprite->getPosition() * this->getScale());
	}
	return Vec2{ 0,0 };
}

void Gun::setGunRotation(float angle)
{
	gunSprite->setRotation(-angle);
}

// PRIVATE METHODS
