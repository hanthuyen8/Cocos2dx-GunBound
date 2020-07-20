#pragma once

#include "cocos2d.h"
#include "Cannon.h"
#include <string_view>
#include "IDamageable.h"

USING_NS_CC;

class Character : public Sprite, public IDamageable
{
public:
	static inline int COLLISION_CATEGORY{ 0x00 };
	static inline int COLLISION_WITH{ 0x00 };

	static Character* create(std::string_view fileName, float radius);
	bool init(std::string_view fileName, float radius);

	// Character sẽ có các thuộc tính:
	CC_SYNTHESIZE(float, moveSpeed, MoveSpeed);

	// Character sẽ có các hàm di chuyển:
	void listenToKeyboardMovement();
	virtual void receiveDamage(const std::vector<Vec2>& damagedPoints) override;

private:
	CC_SYNTHESIZE_READONLY(PhysicsBody*, physicsBody, PhysicsBody);

	float radius{};
	float moveHorizontal{};
	float angle{ };
	Vec2 forwardDirection{ Vec2{1, -1}.getNormalized() };
	Vec2 groundNormal{ 0,-1 };
	bool isKeyPressed{ false };
	bool isFireAndStopMoving{ false };
	Cannon* cannon{};

	virtual void update(float dt) override;
	void onKeyPressed(EventKeyboard::KeyCode key, Event*);
	float checkGround();
	bool onCollisionEnter(PhysicsContact& contact, PhysicsContactPreSolve& solve);
};

