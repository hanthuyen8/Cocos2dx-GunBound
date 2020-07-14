#pragma once

#include "cocos2d.h"

USING_NS_CC;
using PolyVec = std::vector<Vec2>;

// Class này sẽ không quan tâm đến việc class khác xử lý thế nào với dữ liệu của nó
// Nó chỉ làm việc lưu trữ Vec2
class Crate : public ClippingNode
{
public:
	static Crate* createCrate(std::string_view fileName);
	PhysicsBody* getPhysicsBody();
	std::vector<PolyVec> getClippedPoly();
	void replaceShapes(const std::vector<PolyVec>& shapes);
	void eraseArea(const PolyVec& area);

private:
	Sprite* sprite{};
	DrawNode* stencil{};
	PhysicsBody* body{};
	std::vector<PolyVec> clippedPoly{};
};

