#pragma once
#include "cocos2d.h"
USING_NS_CC;

class Cannon : public DrawNode
{
	// Cannon sẽ có nhiệm vụ gì?
	// 1. Nó cần có hàm update để khi có lệnh bắn thì sẽ bắn
	// Bây giờ trước mắt chưa cần cho bắn, mà chỉ cần vẽ drawNode là ổn

	// Vậy trước mắt chỉ cần có variable DrawNode

public:
	static Cannon* createInstance();
	virtual bool init() override;

private:
	float angle{};
	int isAiming{false};
	float isCharging{ false };

	CC_SYNTHESIZE(Vec2, fireDirection, FireDirection);
	CC_SYNTHESIZE(float, force, Force);

	DrawNode* aimMeter{};
	DrawNode* chargeMeter{};
	DrawNode* aimOnAir{};

	virtual void update(float dt) override;

	void drawPath(float dt);

	void onKeyPressed(EventKeyboard::KeyCode, Event*);
	void stopAiming(EventKeyboard::KeyCode, Event*);
};

