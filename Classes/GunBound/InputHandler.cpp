#include "InputHandler.h"
#include "SceneSetup.h"

#pragma region  Public Functions

void InputHandler::controlActor(Character* actor)
{
    actor = actor;
}

void InputHandler::stopControl()
{
    actor = nullptr;
}


#pragma endregion

#pragma region Private Functions

InputHandler::~InputHandler()
{
    actor = nullptr;
    _eventDispatcher->removeEventListenersForTarget(this);
}

bool InputHandler::init()
{
    const auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(InputHandler::onKeyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(InputHandler::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    return true;
}

void InputHandler::onKeyPressed(EventKeyboard::KeyCode key, Event* ev)
{
    if (!actor)
        return;

    switch (key)
    {
        case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
            actor->aim(1);
            break;

        case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
            actor->aim(-1);
            break;

        case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            actor->move(-1);
            break;

        case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            actor->move(1);
            break;

        case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
            actor->startCharge();
            break;
        
        default:
            break;
    }
}

void InputHandler::onKeyReleased(EventKeyboard::KeyCode key, Event* ev)
{
    if (!actor)
        return;

    switch (key)
    {
        case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
            actor->stopAim(1);
            break;

        case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
            actor->stopAim(-1);
            break;

        case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            actor->stopMove(-1);
            break;

        case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            actor->stopMove(1);
            break;

        case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
            actor->stopChargeAndFire();
            break;

        default:
            break;
    }
}

#pragma endregion