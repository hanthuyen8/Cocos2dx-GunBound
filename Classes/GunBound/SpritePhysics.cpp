#include "SpritePhysics.h"
#include "Helper.h"

SpritePhysics* SpritePhysics::createInstance(std::string_view fileName, std::vector<PolyVec>& shape)
{
	auto instance = new SpritePhysics();
	if (instance && instance->init(fileName, shape))
	{
		instance->autorelease();
		return instance;
	}

	CC_SAFE_DELETE(instance);
	return nullptr;
}

bool SpritePhysics::init(std::string_view fileName, std::vector<PolyVec>& shape)
{
	if (!ClippingNode::init())
		return false;

	// Create Stencil
	stencil = DrawNode::create();
	this->addChild(stencil);
	this->setStencil(stencil);
	this->setInverted(true);

	// Create Sprite
	sprite = Sprite::create(std::string{ fileName });
	RETURN_FALSE_IF_NULL_PTR(sprite, "SpritePhysics sprite");
	this->sprite = sprite;
	this->addChild(sprite);

	// Create polygon physics body shape
	physicsBody = PhysicsBody::create();
	physicsBody->setDynamic(false);
	physicsBody->setCategoryBitmask(COLLISION_CATEGORY);
	physicsBody->setContactTestBitmask(COLLISION_WITH);
	replaceShapes(shape);

	this->physicsBody = physicsBody;
	this->addComponent(physicsBody);

	return true;
}

std::vector<PolyVec> SpritePhysics::getClippedPoly()
{
	return clippedPoly;
}

void SpritePhysics::replaceShapes(const std::vector<PolyVec>& shapes)
{
	physicsBody->removeAllShapes();
	clippedPoly.clear();
	clippedPoly = shapes;
	for (const auto& points : shapes)
	{
		physicsBody->addShape(PhysicsShapePolygon::create(points.data(), points.size()));
	}
}

void SpritePhysics::eraseArea(const PolyVec& area)
{
	stencil->drawSolidPoly(area.data(), area.size(), Color4F::BLACK);
}

void SpritePhysics::setDamage(const std::vector<Vec2>& damagedPoints)
{
	CCLOG("SP get dmg");
}
