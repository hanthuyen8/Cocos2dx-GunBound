#pragma once
#include "cocos2d.h"
#include "CustomAnimation.h"
#include "WorldPosition.h"

USING_NS_CC;

class Ammo : public Sprite
{
public:
	static inline int COLLISION_CATEGORY{ 0x00 };
	static inline int COLLISION_WITH{ 0x00 };
	static inline Vec2 ACCELERATION{ Vec2{0, -981} };

	static Ammo* create();
	virtual bool init () override;
	virtual void update(float dt) override;

	/// <summary>
	/// Hàm này trả về danh sách các điểm dự đoán mà đạn sẽ đi qua.
	/// </summary>
	/// <param name="angle"></param>
	/// <param name="speed"></param>
	/// <returns>Danh sách các điểm Vec2</returns>
	//static std::vector<Vec2> Ammo::drawPath(float angle, float speed);


	void fire(float angle, float speed, Vec2& startPoint, float deadZoneRadius);

private:
	CC_SYNTHESIZE(float, radius, Radius);
	float explosionRadius{ 50 };
	PhysicsBody* physicsBody{};
	CustomAnimation* explosionEffect{};
	bool onCollisionEnter(PhysicsContact& contact, PhysicsContactPreSolve& solve);
};