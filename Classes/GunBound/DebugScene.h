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
	PhysicsBody* physicsBody{};
	virtual void update(float dt) override;

};

