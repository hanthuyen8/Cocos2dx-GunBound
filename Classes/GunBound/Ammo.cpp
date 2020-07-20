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

	this->setName("Ammo");
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
	collisionListener->onContactPreSolve = CC_CALLBACK_2(Ammo::onCollisionEnter, this);
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
	this->scheduleUpdate();
	physicsBody->setEnabled(true);
}

bool Ammo::onCollisionEnter(PhysicsContact& contact, PhysicsContactPreSolve& solve)
{
	const auto ammoCat = physicsBody->getCategoryBitmask();
	auto self = contact.getShapeA();
	auto collider = contact.getShapeB();

	if (self->getCategoryBitmask() != ammoCat)
	{
		self = contact.getShapeB();
		collider = contact.getShapeA();

		if (self->getCategoryBitmask() != ammoCat)
			return true;
	}

	IDamageable* target = dynamic_cast<IDamageable*>(collider->getBody()->getNode());
	if (!target)
		return false;

	// Vì lý do PhysicsWorld nó là 1 thành phần gắn liền với 1 Scene cụ thể
	// Cho nên vị trí của Physics Points là vị trí World Space Position + Anchor Position của Scene đó.
	// Vì Scene đã được chuyển sang nằm giữa màn hình thay vì nằm vị trí (0,0)
	// Cho nên phải làm thêm bước convert nữa

	const auto worldSpaceAR_CollisionPoint = contact.getContactData()->points[0];
	const auto sceneAnchor = Director::getInstance()->getRunningScene()->getAnchorPointInPoints();
	const auto worldSpace_CollisionPoint = worldSpaceAR_CollisionPoint - sceneAnchor;

	explosionEffect->playAnimation();

	this->unscheduleUpdate();
	this->setOpacity(0);
	physicsBody->setEnabled(false);

	target->receiveDamage(Helper::getCircle(worldSpace_CollisionPoint, explosionRadius));

	return false;
}
