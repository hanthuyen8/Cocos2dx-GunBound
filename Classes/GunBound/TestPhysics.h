#pragma once

#include "cocos2d.h"

USING_NS_CC;

class TestPhysics : public Scene
{
public:
	static Scene* createScene();
	virtual bool init() override;
	CREATE_FUNC(TestPhysics);

private:
	Sprite* crate{};
};