#include "CustomAnimation.h"

void CustomAnimation::addPlistFile(std::string_view plistfileName, std::string_view patternName)
{
	std::string pattern{ patternName };
	if (CustomAnimation::map.find(pattern) != CustomAnimation::map.end())
		return;

	const std::string plist{ plistfileName };
	const auto cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile(plist);

	std::string fullPath = FileUtils::getInstance()->fullPathForFilename(plist);
	if (fullPath.empty())
	{
		// return if plist file doesn't exist
		CCLOG("cocos2d: SpriteFrameCache: can not find %s", plist.c_str());
		return;
	}

	ValueMap dict = FileUtils::getInstance()->getValueMapFromFile(fullPath);

	if (dict.find("frames") != dict.end())
	{
		ValueMap& metadataDict = dict["frames"].asValueMap();
		map[pattern] = metadataDict.size();
	}
}

CustomAnimation* CustomAnimation::create(std::string_view keyName)
{
	auto anim = new CustomAnimation();
	if (anim && anim->init (keyName))
	{
		anim->autorelease();
		return anim;
	}

	CC_SAFE_DELETE(anim);
	return nullptr;
}

bool CustomAnimation::init (std::string_view keyName)
{
	if (!Sprite::init ())
		return false;

	std::string pattern{ keyName };
	if (CustomAnimation::map.find(pattern) == CustomAnimation::map.end())
	{
		CCLOGWARN("Key: %s không tồn tại.", keyName);
		return false;
	}

	const auto size = map[pattern];
	Vector<SpriteFrame*> listSF{};
	const auto cache = SpriteFrameCache::getInstance();
	animation = Animation::create();
	for (int i{}; i < size; i++)
	{
		const auto sf = cache->getSpriteFrameByName(pattern + std::to_string(i));
		if (sf)
			animation->addSpriteFrame(sf);
	}
	animation->setDelayPerUnit(0.5f / size);
	animation->setRestoreOriginalFrame(true);
	CC_SAFE_RETAIN(animation);

	return true;
}

void CustomAnimation::playAnimation()
{
	this->runAction(Animate::create(animation));
}

CustomAnimation::~CustomAnimation()
{
	CC_SAFE_RELEASE(animation);
}
