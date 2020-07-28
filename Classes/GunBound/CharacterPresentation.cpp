#include "Character.h"
#include "SceneSetup.h"

bool Character::Presentation::init(std::string& animName, const Vec2& spriteAnchor, const Size& spriteSize)
{
	sprite = Sprite::create();
	CC_ASSERT(sprite);
	sprite->setContentSize(spriteSize);
	sprite->setAnchorPoint(spriteAnchor);

	animAttack = animName + AnimSuffix::ATTACK;
	animWalk = animName + AnimSuffix::WALK;
	animIdle = animName + AnimSuffix::IDLE;

	return true;
}

void Character::Presentation::changeAnimation(State newState, bool loop)
{
	switch (newState)
	{
		case Character::State::Move:
			if (currentAnim && *currentAnim == animWalk)
				return;

			currentAnim = &animWalk;
			break;

		case Character::State::Attack:
			if (currentAnim && *currentAnim == animAttack)
				return;

			currentAnim = &animAttack;
			break;

		default:
			if (currentAnim && *currentAnim == animIdle)
				return;

			currentAnim = &animIdle;
			break;
	}

	Animation* anim{ AnimationCache::getInstance()->getAnimation(animIdle) };

	if (anim)
	{
		sprite->stopAllActions();
		const auto animateAction = Animate::create(anim);
		if (loop)
			sprite->runAction(RepeatForever::create(animateAction));
		else
			sprite->runAction(animateAction);
	}
}

void Character::Presentation::setRotation(float angle, bool flipped)
{
	sprite->setRotation(clampf(angle, -90, 90));
	sprite->setFlippedX(flipped);
}
