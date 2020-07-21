#pragma once

#include "cocos2d.h"
#include "IDamageable.h"

USING_NS_CC;
using PolyVec = std::vector<Vec2>;

// Class này sẽ không quan tâm đến việc class khác xử lý thế nào với dữ liệu của nó
// Nó chỉ làm việc lưu trữ Vec2
class SpritePhysics : public ClippingNode, public IDamageable
{
public:
	static inline int COLLISION_CATEGORY{ 0 };
	static inline int COLLISION_WITH{ 0 };

	static SpritePhysics* createInstance(std::string_view fileName, std::vector<PolyVec>& shape);
	bool init (std::string_view fileName, std::vector<PolyVec>& shape);

	std::vector<PolyVec> getShapes();
	void replaceShapes();
	void cutSpriteArea(const PolyVec& area);
	void cutShapeArea(const PolyVec& area);
	virtual void receiveDamage(const std::vector<Vec2>& damagedPoints) override;

private:
	Sprite* sprite{};
	DrawNode* stencil{};
	CC_SYNTHESIZE_READONLY(PhysicsBody*, physicsBody, PhysicsBody);
	std::vector<PolyVec> shapes{};
	bool replaceShapeAtNextFrame{ false };
	virtual void update(float dt) override;
};