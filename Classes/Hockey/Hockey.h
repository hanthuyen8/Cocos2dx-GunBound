#pragma once

#include "cocos2d.h"
#include "GameSprite.h"

USING_NS_CC;

class Hockey : public Layer
{
private:
	std::vector<GameSprite*> players{};
	GameSprite* player1{};
	GameSprite* player2{};
	Sprite* board{};

	Label* player1_ScoreLabel{};
	Label* player2_ScoreLabel{};

	int player1_Score{};
	int player2_Score{};

public:
	static Scene* createScene();
	CREATE_FUNC(Hockey);

	virtual bool init() override;

private:
	void touchBegin(const std::vector<Touch*>& touches, Event* ev);
	void touchMove(const std::vector<Touch*>& touches, Event* ev);
	void touchEnd(const std::vector<Touch*>& touches, Event* ev);

};