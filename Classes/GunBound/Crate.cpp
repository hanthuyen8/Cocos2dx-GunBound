#include "Crate.h"

using namespace ClipperLib;



Crate* Crate::createCrate(std::string_view fileName)
{
	auto instance = new Crate();
	if (instance && instance->initWithFile(fileName.data()))
	{
		instance->autorelease();

		// Create polygon physics body shape
		const int maxSize = 4;
		const auto boundSize = instance->getBoundingBox().size;
		const auto minW = boundSize.width / 2;
		const auto minH = boundSize.height / 2;

		const std::vector<Vec2> rect{ Vec2{ -minW, -minH }, Vec2{ minW, -minH }, Vec2{ minW, minH }, Vec2{ -minW, minH } };

		instance->body = PhysicsBody::createPolygon(rect.data(), maxSize);
		instance->body->setDynamic(false);
		instance->addComponent(instance->body);

		return instance;
	}

	CC_SAFE_DELETE(instance);
	return nullptr;
}

PhysicsBody* Crate::getPhysicsBody()
{
	return body;
}

Paths Crate::getClippedPoly()
{
	return clippedPoly;
}

void Crate::addShapes(const std::vector<std::vector<Vec2>>& shapes)
{
	for (const auto& points : shapes)
	{
		const auto shape = PhysicsShapePolygon::create(points.data(), points.size());
		if (body->addShape(shape))
			tris[shape] = points;
	}
}
