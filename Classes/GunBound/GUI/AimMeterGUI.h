#pragma once
#include "cocos2d.h"

USING_NS_CC;

class AimMeterGUI : public Node
{
public:
	CREATE_FUNC(AimMeterGUI);
	virtual bool init() override;

	void updateAngle(float needleAngle);
	void updatePosition(Vec2& newWPos);

private:
	DrawNode* circle{};
	DrawNode* needle{};
};

