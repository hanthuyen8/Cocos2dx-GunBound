#pragma once

#include "cocos2d.h"
#include "Cannon.h"
#include <string_view>
#include "IDamageable.h"

USING_NS_CC;

class Character : public Node, public IDamageable
{
public:
	static inline int COLLISION_CATEGORY{ 0x00 };
	static inline int COLLISION_WITH{ 0x00 };

	static Character* create(std::string_view fileName, float radius, Vec2 anchor);
	bool init(std::string_view fileName, float radius, Vec2 anchor);

	// Character sẽ có các thuộc tính:
	CC_SYNTHESIZE(float, moveSpeed, MoveSpeed);

	// Character sẽ có các hàm di chuyển:
	void listenToKeyboardMovement();
	virtual void receiveDamage(const std::vector<Vec2>& damagedPoints) override;

	bool raycastHit(PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data);

private:
	CC_SYNTHESIZE_READONLY(PhysicsBody*, physicsBody, PhysicsBody);

	PhysicsShape* isTouchGround{};
	Sprite* sprite{};
	Cannon* cannon{};

	float radius{};
	float moveHorizontal{};
	float angle{ };
	Vec2 groundNormal{ 0,1 };
	bool isKeyPressed{ false };
	bool isFireAndStopMoving{ false };

	virtual void update(float dt) override;
	void onKeyPressed(EventKeyboard::KeyCode key, Event*);
	float findGroundDistanceAndNormal();
	bool onCollisionEnter(PhysicsContact& contact, PhysicsContactPreSolve& solve);
	void onCollisionExit(PhysicsContact& contact);
};

