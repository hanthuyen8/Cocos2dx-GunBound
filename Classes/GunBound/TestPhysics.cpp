#include "TestPhysics.h"
#include "EarCut/earcut.hpp"

Scene* TestPhysics::createScene()
{
	return TestPhysics::create();
}

bool TestPhysics::init()
{
	if (!Scene::initWithPhysics())
		return false;

	// Turn on Physics Debugger
	this->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	this->setPhysics3DDebugCamera(Camera::getDefaultCamera());

	crate = Sprite::create("GunBound/Crate.png");
	crate->setPosition(300, 300);
	this->addChild(crate);

	// Create polygon physics body shape
	const auto bound = crate->getBoundingBox();
	std::vector<std::vector<Vec2>> polygon{};
	polygon.push_back({ Vec2{bound.getMinX(), bound.getMinY()}, Vec2{bound.getMaxX(), bound.getMinY()}, Vec2{bound.getMaxX(), bound.getMaxY()}, Vec2{bound.getMinX(), bound.getMaxY()} });
	std::vector<uint16_t> newTriangles = mapbox::earcut<uint16_t>(polygon);

	std::vector<PhysicsShapePolygon*> shapes{};
	shapes.reserve(newTriangles.size() % 3);

	std::vector<Vec2> vertices{};
	vertices.reserve(3);

	for (int i{}; i < newTriangles.size(); i += 3)
	{
		Vec2 vert[3];
		for (int k{}; k < 3; k++)
		{
			auto index = newTriangles[i + k];
			vert[k] = polygon[0][index];
		}
		shapes.push_back(PhysicsShapePolygon::create(vert, 3));
	}

	const auto physicsBody = PhysicsBody::create();
	for (auto&& shape : shapes)
	{
		physicsBody->addShape(shape);
	}
	physicsBody->setDynamic(true);
	crate->addComponent(physicsBody);

	const auto crate2 = Sprite::create("GunBound/Crate.png");
	crate2->setPosition(300, 0);
	this->addChild(crate2);

	//Vec2 tr[3] = {Vec2(0,0), Vec2(0, 100), Vec2(100, 100)};
	Vec2 tr[3] = { polygon[0][0], polygon[0][2], polygon[0][3] };
	const auto physicsBody2 = PhysicsBody::create();
	physicsBody2->addShape(PhysicsShapeEdgePolygon::create(tr, 3));
	physicsBody2->setDynamic(false);
	crate2->addComponent(physicsBody2);

	return true;
}
