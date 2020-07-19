#include "Ammo.h"
#include "IDamageable.h"
#include "Helper.h"

const std::string PATH_AMMO = "GunBound/Bullet.png";

Ammo* Ammo::create()
{
	Ammo* instance = new(std::nothrow) Ammo();
	if (instance && instance->init())
	{
		instance->autorelease();
		return instance;
	}

	CC_SAFE_DELETE(instance);
	return nullptr;
}

bool Ammo::init()
{
	if (!Sprite::initWithFile(PATH_AMMO))
		return false;

	this->setOpacity(0);

	const auto size = this->getBoundingBox().size;
	radius = MIN(size.width, size.height) / 2;
	physicsBody = PhysicsBody::createCircle(radius, PhysicsMaterial::PhysicsMaterial(0, 0, 0));

	physicsBody->setDynamic(true);
	physicsBody->setGravityEnable(false);

	physicsBody->setCategoryBitmask(COLLISION_CATEGORY);
	physicsBody->setContactTestBitmask(COLLISION_WITH);
	physicsBody->setEnabled(false);

	this->setPhysicsBody(physicsBody);

	explosionEffect = CustomAnimation::create("AnimationSmoke");
	RETURN_FALSE_IF_NULL_PTR(explosionEffect, "AnimationSmoke không khởi tạo được");
	this->addChild(explosionEffect);

	this->unscheduleUpdate();


	const auto collisionListener = EventListenerPhysicsContact::create();
	collisionListener->onContactBegin = CC_CALLBACK_1(Ammo::onCollisionEnter, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(collisionListener, this);

	return true;
}

void Ammo::update(float dt)
{
	Vec2 vel = physicsBody->getVelocity();
	vel += ACCELERATION * dt;
	physicsBody->setVelocity(vel);

	const float angle = MATH_RAD_TO_DEG(vel.getAngle());
	this->setRotation(-angle);
}

void Ammo::fire(float angle, float speed, Vec2& startPoint, float deadZoneRadius)
{
	const float rad{ CC_DEGREES_TO_RADIANS(angle) };
	const Vec2 direction{ std::cos(rad), std::sin(rad) };

	// Set vị trí này để Ammo không va phạm trúng Character đã bắn nó
	startPoint = this->getParent()->convertToNodeSpace(startPoint);
	this->setPosition(startPoint + direction * (deadZoneRadius + radius));
	this->setOpacity(255);

	physicsBody->setVelocity(direction * speed);
	physicsBody->setEnabled(true);
	this->scheduleUpdate();
}

bool Ammo::onCollisionEnter(PhysicsContact& contact)
{
	Node* collider = contact.getShapeB()->getBody()->getNode();

	IDamageable* target = dynamic_cast<IDamageable*>(collider);
	if (!target)
		return false;

	const auto pos = contact.getContactData()->points[0];
	explosionEffect->playAnimation();

	Helper::logVec2(pos);
	target->setDamage(Helper::getCircle(pos, explosionRadius));
	this->unscheduleUpdate();
	this->setOpacity(0);
	physicsBody->setEnabled(false);

	return true;
}
