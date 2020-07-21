#pragma once
#include "cocos2d.h"
#include "Ammo.h"

USING_NS_CC;

class Cannon : public DrawNode
{
	// Cannon sẽ có nhiệm vụ gì?
	// 1. Nó cần có hàm update để khi có lệnh bắn thì sẽ bắn
	// Bây giờ trước mắt chưa cần cho bắn, mà chỉ cần vẽ drawNode là ổn

	// Vậy trước mắt chỉ cần có variable DrawNode

public:
	static Cannon* create (float deadZoneRadius);
	bool init (float deadZoneRadius);

private:
	float angle{};
	float deadZoneRadius{};
	int isAiming{ false };
	float isCharging{ false };

	DrawNode* aimMeter{};
	DrawNode* chargeMeter{};
	DrawNode* ammoPath{};
	Ammo* ammo{};

	virtual void update(float dt) override;

	void fire();

	void onKeyPressed(EventKeyboard::KeyCode, Event*);
	void onKeyReleased(EventKeyboard::KeyCode, Event*);
	std::vector<Vec2> drawPath(float angle, float speed);
	void tryToCreateAmmo();
};

