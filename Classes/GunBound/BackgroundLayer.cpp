#include "BackgroundLayer.h"

BackgroundLayer* BackgroundLayer::create()
{
    auto layer = new BackgroundLayer();
    if (layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }

    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool BackgroundLayer::init()
{
    if (!Layer::init())
        return false;


    return true;
}

void BackgroundLayer::moveByCharacterPosition(Vec2 characterWPos)
{
    // BackgroundLayer sẽ giữ vị trí khớp với vị trí của character nhưng sẽ không thể lên cao hoặc xuống thấp
    auto pos = this->getPosition();
    pos.x = characterWPos.x;
    this->setPosition(pos);
}
