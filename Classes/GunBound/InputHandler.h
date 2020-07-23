#pragma once
#include "cocos2d.h"

USING_NS_CC;

class InputHandler : public Ref
{
public:
	static InputHandler* getInstance();

private:
	inline static InputHandler* instance{};
	bool init();
	void onKeyPressed(EventKeyboard::KeyCode key, Event* ev);
	void onKeyReleased(EventKeyboard::KeyCode key, Event* ev);
};

