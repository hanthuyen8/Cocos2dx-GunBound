#pragma once
#include "cocos2d.h"
USING_NS_CC;

class Ammo : public Sprite
{
public:
	static inline int COLLISION_CATEGORY{ 0x00 };
	static inline int COLLISION_WITH{ 0x00 };
	static inline Vec2 ACCELERATION{ Vec2{0, -9.81} };

	static Ammo* create();
	virtual bool init() override;
	virtual void update(float dt) override;

	std::vector<Vec2> Ammo::drawPath(float angle, float force, float dt);
	void fire(float angle, float force);
	void fireAndDrawPath(float angle, float force);

private:
	enum class AmmoActionType
	{
		None = 0x00, Fire = 0x01, DrawPath = 0x02, FireAndDrawPath = Fire | DrawPath
	};

	AmmoActionType ammoAction{};
	PhysicsBody* physicsBody{};
	float explosionRadius{ 50 };
	float angle{};
	float force{};
	Vec2 velocity{};

	void setAmmoSpecs(float angle, float force);
	bool onCollisionEnter(PhysicsContact& contact);
};