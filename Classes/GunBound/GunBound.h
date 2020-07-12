#pragma once
#include "cocos2d.h"

USING_NS_CC;

class GunBound : public Scene
{
private:
	int force{ 70 };
	Vec2 direction{ 0,0 };
	const Vec2 acceleration{ 0, -9.81 };
	Label* forceLabel{};
	Label* directionLabel{};
	Label* normalizeLabel{};
	std::vector<DrawNode*> dots{};
	virtual void update(float dt) override;

public:
	static Scene* createScene();
	CREATE_FUNC(GunBound);
	virtual bool init() override;
};

