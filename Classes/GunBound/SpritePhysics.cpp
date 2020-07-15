#include "SpritePhysics.h"

SpritePhysics* SpritePhysics::createInstance(std::string_view fileName, std::vector<PolyVec>&& shape)
{
	auto instance = new SpritePhysics();
	if (instance)
	{
		instance->autorelease();

		// Create Stencil
		const auto stencil = DrawNode::create();
		instance->stencil = stencil;
		instance->addChild(stencil);
		instance->setStencil(stencil);
		instance->setInverted(true);

		// Sprite
		const auto sprite = Sprite::create(std::string{ fileName });
		instance->sprite = sprite;
		instance->addChild(sprite);

		// Create polygon physics body shape
		const auto physicsBody = PhysicsBody::create();
		instance->physicsBody = physicsBody;
		physicsBody->setDynamic(false);
		instance->addComponent(physicsBody);
		instance->replaceShapes(shape);

		return instance;
	}

	CC_SAFE_DELETE(instance);
	return nullptr;
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

void SpritePhysics::setCollisionMask(int selfMask, int collideWith)
{
	physicsBody->setCategoryBitmask(selfMask);
	physicsBody->setCollisionBitmask(collideWith);
}
