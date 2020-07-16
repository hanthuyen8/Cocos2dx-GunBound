#include "Cannon.h"
#include "GunBound/GunBoundScene.h"

Cannon* Cannon::createInstance()
{
	Cannon* cannon = new Cannon();
	if (cannon && cannon->init())
	{
		cannon->autorelease();
		return cannon;
	}

	CC_SAFE_DELETE(cannon);
	return nullptr;
}

bool Cannon::init()
{
	if (!DrawNode::init())
		return false;

	return true;
}

void Cannon::aiming(Vec2& direction, float force, float dt)
{
	this->clear();

	// Vẽ lên các chấm tròn biểu thị cho projectile của 1 viên cannon ball.
	// Số chấm cần vẽ là 10.
	const int numberOfDot{ 50 };
	Vec2 velocity{ direction * force };
	Vec2 start{ this->getPosition() };
	Vec2 acceleration{ GunBoundScene::getAcceleration() };
	int i{ };
	this->drawCircle(Vec2::ZERO, 10, 10, 10, false, Color4F::WHITE);
	return;
	
	do
	{
		velocity += acceleration * dt;
		start += velocity * dt;

		if (i++ % numberOfDot != 0)
			continue;

		// Draw Dot
		this->drawSolidCircle(start, 10, 360, 10, 1, 1, Color4F::WHITE);
	} while (start.y > 0);
}
