#pragma once

#include "cocos2d.h"
#include <string_view>
#include <iomanip>

USING_NS_CC;
using KeyCode = EventKeyboard::KeyCode;

class GameScene : public Scene
{
private:
	Sprite* character{ nullptr };
	const float moveSpeed{ 300 };

public:
	static Scene* createScene();
	virtual bool init() override;

	virtual void update(float dt) override;
	CREATE_FUNC(GameScene);

private:
	KeyCode keyPressed{ KeyCode::KEY_NONE };

	void onKeyDown(KeyCode key, Event* ev);
	void onKeyUp(KeyCode key, Event* ev);
	//Vector<SpriteFrame*> getSpriteFrameFromCache(std::string_view atlasName, int size);
};

