#pragma once

#include "cocos2d.h"
#include "Cannon.h"
#include "IDamageable.h"

USING_NS_CC;



class Character : public Node, public IDamageable
{
public:
	static inline int COLLISION_CATEGORY{ 0x00 };
	static inline int COLLISION_WITH{ 0x00 };

	static Character* create(std::string_view name, Vec2& spriteAnchor, Size& spriteSize, float physicsRadius);
	bool init(std::string_view name, Vec2& spriteAnchor, Size& spriteSize, float physicsRadius);

	// Character sẽ có các thuộc tính:
	CC_SYNTHESIZE(float, moveSpeed, MoveSpeed);
	bool cameraFollow{ true };

	void listenToKeyboardMovement();
	virtual void receiveDamage(const std::vector<Vec2>& damagedPoints) override;
	void setOnCharacterMovementCallback(std::function<void(Vec2 characterWPos)> callback);

private:
	enum class AnimState {
		Idle, Walk, Attack
	};

	CC_SYNTHESIZE_READONLY(PhysicsBody*, physicsBody, PhysicsBody);

	PhysicsShape* isTouchGround{};
	Sprite* sprite{};
	Cannon* cannon{};

	std::string name{};
	float radius{};
	float moveHorizontal{};
	float angle{ };
	Vec2 groundNormal{ 0,1 };
	bool isFireAndStopMoving{ false };
	std::vector<std::function<void(Vec2 characterWPos)>> characterMovementCallback;

	// Animations
	Animate* animateAction{};

	// Functions
	virtual void update(float dt) override;

	void onKeyPressed(EventKeyboard::KeyCode key, Event*);
	void onKeyReleased(EventKeyboard::KeyCode key, Event*);
	void doPlayerMovementCallback();
	void changeAnimation(AnimState state, bool loop);

	float findGroundDistanceAndNormal();
	bool onCollisionEnter(PhysicsContact& contact, PhysicsContactPreSolve& solve);
	void onCollisionExit(PhysicsContact& contact);
	bool raycastHit(PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data);
};

