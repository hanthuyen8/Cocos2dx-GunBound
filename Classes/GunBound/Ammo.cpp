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
	physicsBody = PhysicsBody::createCircle(MIN(size.width, size.height), PhysicsMaterial::PhysicsMaterial(1, 0, 1));

	physicsBody->setDynamic(true);
	physicsBody->setGravityEnable(false);

	physicsBody->setCategoryBitmask(COLLISION_CATEGORY);
	physicsBody->setContactTestBitmask(COLLISION_WITH);
	physicsBody->setEnabled(false);

	this->addComponent(physicsBody);

	return true;
}

void Ammo::update(float dt)
{
	if (ammoAction == AmmoActionType::Fire)
	{
		Vec2 pos = physicsBody->getVelocity();
		velocity += ACCELERATION * dt;
		pos += velocity * dt;

		physicsBody->setVelocity(pos);
	}
}

const int MAX_LOOP = 5000;
std::vector<Vec2> Ammo::drawPath(float angle, float force, float dt)
{
	setAmmoSpecs(angle, force);
	ammoAction = AmmoActionType::DrawPath;

	const int loopSkip{ 50 };
	const float rad{ CC_DEGREES_TO_RADIANS(angle) };
	const Vec2 direction{ std::cos(rad), std::sin(rad) };
	const Vec2 acceleration{ ACCELERATION * dt };
	Vec2 velocity{ direction * force };
	Vec2 pos{ Vec2::ZERO };
	std::vector<Vec2> points{};
	int i{ };

	// Tính trước xem cần bao nhiêu vòng lặp để đưa pos.y về bé hơn 0:

	const float decreaseYPerTick{ acceleration.y * dt };
	int loopAmountNeed = 0;
	if (decreaseYPerTick != 0)
		loopAmountNeed = std::fabs(velocity.y / decreaseYPerTick);
	if (decreaseYPerTick >= 0 || loopAmountNeed > MAX_LOOP)
	{
		// Với chỉ số ACCELERATION này thì pos.y sẽ không bao giờ bé hơn 0. (decreaseYPerTick >= 0)
		// Hoặc chỉ số loopAmountNeed quá lớn.
		CCLOGWARN("ACCELERATION không âm: (%f, %f)", ACCELERATION.x, ACCELERATION.y);
		return points;
	}

	for (int i{}; i < loopAmountNeed; i++)
	{
		velocity += acceleration;
		pos += velocity * dt;

		if (i % loopSkip != 0)
			continue;

		points.push_back(pos);
	}
	return points;
}

void Ammo::fire(float angle, float force)
{
	setAmmoSpecs(angle, force);
	this->setPosition(Vec2::ZERO);
	this->setOpacity(255);
	physicsBody->setEnabled(true);

	ammoAction = AmmoActionType::Fire;

	const float rad{ CC_DEGREES_TO_RADIANS(angle) };
	const Vec2 direction{ std::cos(rad), std::sin(rad) };
	velocity = direction * force;

	const auto collisionListener = EventListenerPhysicsContact::create();
	collisionListener->onContactBegin = CC_CALLBACK_1(Ammo::onCollisionEnter, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(collisionListener, this);

	this->scheduleUpdate();
}

void Ammo::fireAndDrawPath(float angle, float force)
{
	setAmmoSpecs(angle, force);
	ammoAction = AmmoActionType::FireAndDrawPath;
	this->scheduleUpdate();
}

void Ammo::setAmmoSpecs(float angle, float force)
{
	this->angle = angle;
	this->force = force;
}

bool Ammo::onCollisionEnter(PhysicsContact& contact)
{
	Node* collider = contact.getShapeB()->getBody()->getNode();
	IDamageable* target = dynamic_cast<IDamageable*>(collider);
	if (!target)
		return false;

	const auto pos = contact.getContactData()->points[0];
	Helper::logVec2(pos);
	target->setDamage(Helper::getCircle(pos, explosionRadius));
	this->unscheduleUpdate();
	this->setOpacity(0);
	physicsBody->setEnabled(false);

	return true;
}
