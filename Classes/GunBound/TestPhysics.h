#pragma once

#include "cocos2d.h"
#include "Crate.h"
#include "clipper.hpp"

USING_NS_CC;

class TestPhysics : public Scene
{
public:
	static Scene* createScene();
	virtual bool init() override;
	CREATE_FUNC(TestPhysics);

private:
	Crate* crate{};
	void onCut(EventMouse* ev);
	void onCut2(EventMouse* ev);
	DrawNode* drawNode{};
};