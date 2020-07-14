#pragma once

#include "cocos2d.h"
#include "clipper.hpp";

USING_NS_CC;

class Crate : public Sprite
{
public:
	static Crate* createCrate(std::string_view fileName);
	PhysicsBody* getPhysicsBody();
	Paths getClippedPoly();
	void addShapes(const std::vector<std::vector<Vec2>>& shapes);

private:
	PhysicsBody* body{};
	Paths clippedPoly{};
};

