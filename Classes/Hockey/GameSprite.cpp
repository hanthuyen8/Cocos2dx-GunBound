#include "GameSprite.h"

GameSprite* GameSprite::createGameSprite(std::string_view fileName)
{
    auto gameSprite = new GameSprite();
    if (gameSprite && gameSprite->initWithSpriteFrameName(fileName.data()))
    {
        gameSprite->autorelease();
        return gameSprite;
    }

    CC_SAFE_DELETE(gameSprite);
    return nullptr;
}
