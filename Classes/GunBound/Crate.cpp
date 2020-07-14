#include "Crate.h"

Crate* Crate::createCrate(std::string_view fileName)
{
	auto instance = new Crate();
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
		const int maxSize = 4;
		const auto boundSize = sprite->getBoundingBox().size;
		const auto minW = boundSize.width / 2;
		const auto minH = boundSize.height / 2;

		const std::vector<Vec2> rect{ Vec2{ -minW, -minH }, Vec2{ minW, -minH }, Vec2{ minW, minH }, Vec2{ -minW, minH } };
		instance->clippedPoly = { rect };

		const auto physicsBody = PhysicsBody::createPolygon(rect.data(), maxSize);
		instance->body = physicsBody;
		physicsBody->setDynamic(false);
		instance->addComponent(physicsBody);

		return instance;
	}

	CC_SAFE_DELETE(instance);
	return nullptr;
}

PhysicsBody* Crate::getPhysicsBody()
{
	return body;
}

std::vector<PolyVec> Crate::getClippedPoly()
{
	return clippedPoly;
}

void Crate::replaceShapes(const std::vector<PolyVec>& shapes)
{
	body->removeAllShapes();
	clippedPoly.clear();
	clippedPoly = shapes;
	for (const auto& points : shapes)
	{
		body->addShape(PhysicsShapePolygon::create(points.data(), points.size()));
	}
}

void Crate::eraseArea(const PolyVec& area)
{
	stencil->drawSolidPoly(area.data(), area.size(), Color4F::BLACK);
}
