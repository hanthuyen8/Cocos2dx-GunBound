#pragma once
#include "cocos2d.h"
#include "Character.h"

USING_NS_CC;

class InputHandler : public Node
{
public:
	CREATE_FUNC(InputHandler);
	void controlActor(Character* actor);
	void stopControl();

private:
	~InputHandler();
	virtual bool init() override;
	void onKeyPressed(EventKeyboard::KeyCode key, Event* ev);
	void onKeyReleased(EventKeyboard::KeyCode key, Event* ev);
	Character* actor;
};

