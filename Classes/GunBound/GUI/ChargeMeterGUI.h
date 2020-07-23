#pragma once

#include "cocos2d.h"

USING_NS_CC;

class ChargeMeterGUI : DrawNode
{
public:
	CREATE_FUNC(ChargeMeterGUI);
	virtual bool init() override;
	void show(Vec2 atWPos);
	void updateAngle(float angle);
	void setFlipX(bool left);
	void hide();

private:
	DrawNode* needle{};
};

