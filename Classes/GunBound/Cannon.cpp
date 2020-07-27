#include "Cannon.h"
#include "GunBound/GunBoundScene.h"
#include "Helper.h"
#include "SceneSetup.h"

const float MIN_ANGLE = 0;
const float MAX_ANGLE = 180;
const float AIM_SPEED = 200;
const float CHARGE_SPEED = 70;
const float MAX_SPEED = 1350;

float Character::Cannon::aim(float dt)
{
	aimAngle = clampf(aimAngle + aimDirection * AIM_SPEED * dt, MIN_ANGLE, MAX_ANGLE);
	return aimAngle;
}

float Character::Cannon::charge(float dt)
{
	chargePower = clampf(chargePower + CHARGE_SPEED * dt, 0, 100);
	return chargePower;
}

std::vector<Vec2> Character::Cannon::simulateAmmoPath(Vec2 initDisplacement, float angle, float speed)
{
	const Vec2 unitVector{ ProjectileMotion2D::calc_unitVector(angle) };
	const Vec2 initVelocity{ unitVector * speed };
	const Vec2& acceleration = Ammo::ACCELERATION;
	// B1. Tìm thời gian mà đạn sẽ tiếp đất
	// B2. Dựa vào thời gian đó, chia nhỏ ra mỗi 1 giây sẽ vẽ 1 chấm tương ứng với vị trí nào

	const auto totalTime = ProjectileMotion2D::find_t(initDisplacement.y, initVelocity.y, acceleration.y);

	std::vector<Vec2> path{};
	for (float sec{ 0 }; sec < totalTime; sec += 0.1f)
	{
		const auto point = ProjectileMotion2D::find_d(initVelocity, acceleration, sec, initDisplacement);
		path.push_back(point);
	}
	return path;
}
