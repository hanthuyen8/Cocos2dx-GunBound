#include "SpritePhysics.h"
#include "Helper.h"
#include "clipper.hpp"
#include "EarCut/earcut.hpp"

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
	shapes = shape;
	replaceShapes();

	this->physicsBody = physicsBody;
	this->addComponent(physicsBody);

	return true;
}

void SpritePhysics::update(float dt)
{
	if (replaceShapeAtNextFrame)
	{
		replaceShapes();
		replaceShapeAtNextFrame = false;
	}
}

std::vector<PolyVec> SpritePhysics::getShapes()
{
	return shapes;
}

void SpritePhysics::replaceShapes()
{
	physicsBody->removeAllShapes();
	for (const auto& points : shapes)
	{
		physicsBody->addShape(PhysicsShapePolygon::create(points.data(), points.size(), PhysicsMaterial::PhysicsMaterial(0, 0, 0)));
	}
	physicsBody->setCategoryBitmask(SpritePhysics::COLLISION_CATEGORY);
	physicsBody->setContactTestBitmask(SpritePhysics::COLLISION_WITH);
}

void SpritePhysics::cutSpriteArea(const PolyVec& area)
{
	stencil->drawSolidPoly(area.data(), area.size(), Color4F::WHITE);
}

void SpritePhysics::cutShapeArea(const PolyVec& area)
{
	// Cho Clip trước rồi mới chuyển sang triangle;
	ClipperLib::Clipper clipper{};
	clipper.AddPath(ClipperLib::vecToPath(area), ClipperLib::PolyType::ptClip, true);

	for (auto& shape : shapes)
	{
		clipper.AddPath(ClipperLib::vecToPath(shape), ClipperLib::PolyType::ptSubject, true);
	}

	ClipperLib::Paths result;
	clipper.Execute(ClipperLib::ClipType::ctDifference, result);

	// Sau khi Clip xong thì Convert poly sang triangle

	std::vector<PolyVec> triangles{};
	for (auto& path : result)
	{
		const auto vec = ClipperLib::pathToVec(path);
		std::vector<PolyVec> clipContainer{ vec };

		const auto one_triangle = mapbox::getTrianglesFromPoly(vec, mapbox::earcut<uint32_t>(clipContainer));
		triangles.insert(triangles.begin(), one_triangle.begin(), one_triangle.end());
	}

	shapes.clear();
	shapes = triangles;
	replaceShapeAtNextFrame = true;
}

void SpritePhysics::receiveDamage(const std::vector<Vec2>& damagedPoints)
{
	const auto parent = this->getParent();
	PolyVec area = {};
	area.reserve(damagedPoints.size());

	for (auto vec : damagedPoints)
	{
		area.push_back(this->convertToNodeSpace(vec));
	}

	cutSpriteArea(area);
	cutShapeArea(area);
	CCLOG("SP get dmg");
}
