#pragma once
#include "cocos2d.h"
USING_NS_CC;

class DebugScene : public Scene
{
public:
	static Scene* createScene();
	virtual bool init() override;
	CREATE_FUNC(DebugScene);

private:
	DrawNode* drawNode{};
	Label* lblAngleDeg{};
	Label* lblAngleRad{};
	Label* lblNormal{};
	Label* lblXAxisAngle{};

	float angle{};

};

