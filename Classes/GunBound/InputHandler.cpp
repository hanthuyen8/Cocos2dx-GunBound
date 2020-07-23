#include "InputHandler.h"
#include "SceneSetup.h"

InputHandler* InputHandler::getInstance()
{
    if (instance)
        return instance;

    instance = new InputHandler();
    if (instance && instance->init())
    {
        instance->autorelease();
        return instance;
    }
    CC_SAFE_DELETE(instance);
    return nullptr;
}

bool InputHandler::init()
{
    const auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(InputHandler::onKeyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(InputHandler::onKeyReleased, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(keyboardListener, 0);

    return true;
}

void InputHandler::onKeyPressed(EventKeyboard::KeyCode key, Event* ev)
{
    switch (key)
    {
        case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
            break;

        case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
            break;

        case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            break;

        case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            break;

        case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
            break;
        
        default:
            break;
    }
}

void InputHandler::onKeyReleased(EventKeyboard::KeyCode key, Event* ev)
{

}
