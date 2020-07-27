#include "Character.h"
#include "Helper.h"
#include "SpritePhysics.h"

const int FALL_SPEED = 981;
const int MAX_SLOPE_ANGLE = 60;

void Character::Movement::init(PhysicsWorld* world, float radius)
{
	physicsBody = PhysicsBody::create();
	physicsBody->addShape(PhysicsShapeCircle::create(radius, PhysicsMaterial::PhysicsMaterial(0, 0, 0)));
	physicsBody->setDynamic(true);
	physicsBody->setGravityEnable(false);
	physicsBody->setRotationEnable(true);

	physicsBody->setCategoryBitmask(COLLISION_CATEGORY);
	physicsBody->setContactTestBitmask(COLLISION_WITH);

	physicsWorld = world;
}

// Return false nếu không có di chuyển trong frame này
bool Character::Movement::updatePosition(int nextMoveDirectionX, float dt)
{
	const auto groundDistance = findGroundDistanceAndNormal(nextMoveDirectionX);
	const auto velocity = physicsBody->getVelocity();


	if (nextMoveDirectionX != 0)
	{
		// Khi Character có lệnh di chuyển

		// Gravity nghiêng theo mặt đất
		auto gravity = FALL_SPEED * -1 * groundNormal;
		physicsBody->applyForce(gravity);

		// clamp velocity.x không cho vượt quá moveSpeed
		auto force_x = std::abs(velocity.x);
		if (nextMoveDirectionX == lastMoveDirectionX)
		{
			// Phát hiện chuyển hướng
			// Quất ngay 1 lực hướng ngược lại để chống trượt dài theo velocity cũ
			force_x = velocity.x * -1 / dt;
		}
		else
		{
			// Không phát hiện chuyển hướng
			force_x = (moveSpeed - force_x) * nextMoveDirectionX;
		}
		physicsBody->applyForce(Vec2{ force_x ,0 });
		lastMoveDirectionX = nextMoveDirectionX;
		return true;
	}
	else
	{
		// Khi Character không có lệnh di chuyển -> đứng yên

		if (!isTouchGround && groundDistance > 2)
		{
			// Gravity hút xuống đất
			float y = -FALL_SPEED;

			// hãm lại x
			float x = -std::abs(velocity.x);

			physicsBody->applyForce(Vec2{ x, y });
			return true;
		}
		else
		{
			physicsBody->setVelocity(Vec2::ZERO);
			return false;
		}
	}
}

float Character::Movement::getRotation()
{
	if(groundNormal == Vec2::ZERO)
	return 0.0f;

	//sprite->setRotation(clampf(angle, -90, 90));
	return 90 - MATH_RAD_TO_DEG(groundNormal.getAngle());
}

bool Character::Movement::raycastHit(PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data)
{
	if (info.shape->getBody() == physicsBody)
	{
		// self groundDetected
		return true;
	}
	*(static_cast<PhysicsRayCastInfo*>(data)) = info;
	return false;
}

float Character::Movement::findGroundDistanceAndNormal(float faceDirection)
{
	if (!physicsWorld)
		return 0;

	const auto rayLength = 100;

	PhysicsRayCastInfo groundInfo{};
	PhysicsRayCastInfo slopeInfo{};

	const Vec2 origin{ physicsBody->getPosition() };
	const auto func = CC_CALLBACK_3(Character::Movement::raycastHit, this);

	/*
	Cách hoạt động:
	- Bắn 2 raycast để lấy ground & slope.
	- Kẻ 1 đường nối 2 điểm contact đó, đường đó được xem như là ground mới.
	- Lấy groundNormal và fraction như sau:
		- Nếu slope fraction <= 0 thì lấy normal và fraction của ground;
		- Nếu slope normal == groundNormal thì lấy normal và fraction của ground;
		- Nếu slope normal != groundNormal thì lấy normal mới và trung bình cộng của 2 fraction.
	*/

	if (faceDirection)
	{
		// Raycast to detect Slope
		const Vec2 rayToSlopeStart{ origin + Vec2::UNIT_X * (radius + 1) * faceDirection };
		const Vec2 rayToSlopeEnd{ rayToSlopeStart - groundNormal * rayLength };
		physicsWorld->rayCast(func, rayToSlopeStart, rayToSlopeEnd, &slopeInfo);
	}

	// Raycast to detect Ground
	const Vec2 rayToGroundStart{ origin - Vec2::UNIT_Y * (radius + 1) };
	const Vec2 rayToGroundEnd{ Vec2{rayToGroundStart.x, rayToGroundStart.y - rayLength} };
	//const Vec2 rayToGroundEnd{ origin - Vec2::UNIT_Y * (radius + rayLength) };
	physicsWorld->rayCast(func, rayToGroundStart, rayToGroundEnd, &groundInfo);

	/*
		- Nếu slope fraction <= 0 thì lấy normal và fraction của ground;
		- Nếu slope normal == groundNormal thì lấy normal và fraction của ground;
		- Nếu slope normal != groundNormal thì lấy normal mới và trung bình cộng của 2 fraction.
	*/
	Vec2 resultNormal{};
	float resultFraction{};

	const auto setResult = [&resultNormal, &resultFraction, rayLength](PhysicsRayCastInfo& info) {
		resultNormal = info.normal;
		resultFraction = info.fraction * rayLength;
	};

	const bool groundDetected{ groundInfo.shape != nullptr && groundInfo.normal.y > 0 };
	const bool slopeDetected{ slopeInfo.shape != nullptr && slopeInfo.normal.y > 0 };

	if (!groundDetected)
	{
		// Ko groundDetected trúng Ground -> Lấy y
		resultNormal = Vec2::UNIT_Y;
		resultFraction = rayLength;
	}
	else
	{
		if (!slopeDetected || slopeInfo.normal == groundInfo.normal)
		{
			// Nếu không bắn trúng Slope
			// Hoặc bắn trúng nhưng Slope và Ground cùng phương
			setResult(groundInfo);
		}
		else
		{
			// Slope và Ground khác phương
			// Nhiều trường hợp nếu vẫn để y thì Character sẽ bị stuck ở đáy hoặc ở đỉnh của polygon
			// Bỏ luôn y ở trường hợp này cho khỏe
			groundNormal = Vec2::ZERO;
			return 0;
		}
	}

	groundNormal = resultNormal;

	return resultFraction;
}

// Mục đích của hàm này là xác định Character đã chạm đất hay chưa thôi
bool Character::Movement::onCollisionEnter(PhysicsContact& contact, PhysicsContactPreSolve& solve)
{
	PhysicsShape* self{};
	PhysicsShape* other{};
	if (!Helper::detectWhichCollider(contact, self, other, Character::COLLISION_CATEGORY))
		return true;

	if (self->getBody() != physicsBody)
		return true;

	const auto terrain = dynamic_cast<SpritePhysics*>(other->getBody()->getNode());
	if (!terrain)
		return true;

	isTouchGround = other;
	return true;
}

// Nếu Character rời khỏi shape của mặt đất mà nó đã đứng trước đó thì mới được tính là đã rời khỏi mặt đất
void Character::Movement::onCollisionExit(PhysicsContact& contact)
{
	PhysicsShape* self{};
	PhysicsShape* other{};
	if (!Helper::detectWhichCollider(contact, self, other, Character::COLLISION_CATEGORY))
		return;

	if (self->getBody() != physicsBody)
		return;

	const auto terrain = dynamic_cast<SpritePhysics*>(other->getBody()->getNode());
	if (!terrain)
		return;

	if (isTouchGround && other == isTouchGround)
		isTouchGround = nullptr;
}