#pragma once

#include "cocos2d.h"

USING_NS_CC;
using PolyVec = std::vector<Vec2>;

// Class này sẽ không quan tâm đến việc class khác xử lý thế nào với dữ liệu của nó
// Nó chỉ làm việc lưu trữ Vec2
class SpritePhysics : public ClippingNode
{
public:
	static SpritePhysics* createInstance(std::string_view fileName, std::vector<PolyVec>&& shape);
	std::vector<PolyVec> getClippedPoly();
	void replaceShapes(const std::vector<PolyVec>& shapes);
	void eraseArea(const PolyVec& area);
	void setCollisionMask(int selfMask, int collideWith);

private:
	Sprite* sprite{};
	DrawNode* stencil{};
	CC_SYNTHESIZE_READONLY(PhysicsBody*, physicsBody, PhysicsBody);
	std::vector<PolyVec> clippedPoly{};
};

