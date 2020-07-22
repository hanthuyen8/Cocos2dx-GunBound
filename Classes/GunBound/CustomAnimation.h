#pragma once
#include "cocos2d.h"
#include <string_view>
#include <map>
USING_NS_CC;

class CustomAnimation : public Sprite
{
public:
	static void addPlistFile(std::string_view plistfileName, std::string_view keyName);
	static CustomAnimation* create(  (std::string_view keyName);
	bool init (std::string_view keyName);
	void playAnimation();

private:
	inline static std::map<std::string, int> map{};
	Animation* animation{};
	~CustomAnimation();
};

