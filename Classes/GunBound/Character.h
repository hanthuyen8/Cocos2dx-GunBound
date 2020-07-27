#pragma once

#include "cocos2d.h"
#include "IDamageable.h"

USING_NS_CC;

class Character : public Node, public IDamageable
{
private:
	enum class State
	{
		Idle, Move, Attack, Aim, Charge
	};

	/// <summary>
	/// Class này có nhiệm vụ xử lý các tác vụ với Physics, Position, Rotation
	/// </summary>
	class Movement
	{
	public:
		void init(PhysicsWorld* world, float radius);
		inline PhysicsBody* getPhysicsBody() const { return physicsBody; };

		/// <summary>
		/// Hàm này sẽ cập nhật vị trí của Character
		/// </summary>
		/// <param name="character">Character cần di chuyển</param>
		/// <param name="dt">delta Time</param>
		/// <returns>Return true: có thay đổi vị trí; false: không có thay đổi vị trí</returns>
		bool updatePosition(int nextMoveDirectionX, float dt);
		float getRotation();

	private:
		PhysicsWorld* physicsWorld{};
		PhysicsBody* physicsBody{};
		PhysicsShape* isTouchGround{};
		float lastMoveDirectionX{ 0 };
		float radius{};
		float moveSpeed{ 450 };
		Vec2 groundNormal{ 0,1 };

		float findGroundDistanceAndNormal(float faceDirection);
		bool onCollisionEnter(PhysicsContact& contact, PhysicsContactPreSolve& solve);
		void onCollisionExit(PhysicsContact& contact);
		bool raycastHit(PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data);
	};

	class Presentation
	{
	public:
		void init(std::string& animName, const Vec2& spriteAnchor, const Size& spriteSize);
		inline Sprite* getSprite() const { return sprite; };
		void changeAnimation(State newState, bool loop);
		void setRotation(float angle, bool flipped);

	private:
		Sprite* sprite{};
		std::string& currentAnim;
		std::string animIdle{};
		std::string animAttack{};
		std::string animWalk{};
	};

	class Cannon
	{
	public:
		float aim(float dt);
		float charge(float dt);
		std::vector<Vec2> simulateAmmoPath(Vec2 initDisplacement, float angle, float speed);
		inline float getAimAngle() { return aimAngle; };
		inline float getChargePower() { return chargePower; };

	private:
		CC_SYNTHESIZE(int, aimDirection, AimDirection);
		float aimAngle{};
		float chargePower{};
	};

public:
	static inline int COLLISION_CATEGORY{ 0x00 };
	static inline int COLLISION_WITH{ 0x00 };

	static Character* create(std::string_view characterName, const Vec2& spriteAnchor, const Size& spriteSize, float physicsRadius);
	bool init(std::string_view characterName, const Vec2& spriteAnchor, const Size& spriteSize, float physicsRadius);

	// Character sẽ có các thuộc tính:
	bool cameraFollow{ true };

	virtual void receiveDamage(const std::vector<Vec2>& damagedPoints) override;
	void setOnCharacterMoveCallback(const std::function<void(Vec2 characterWPos)> callback);
	void setOnCharacterAimCallback(const std::function<void(float aimAngle)> callback);
	void setOnCharacterChargeCallback(const std::function<void(float chargePower)> callback);


	// Các hàm điều khiển
	void move(int directionX);
	void stopMove(int directionX);
	void aim(int directionY);
	void stopAim(int directionY);
	void startCharge();
	void stopChargeAndFire();

private:

	std::unique_ptr<Movement> characterMovement;
	std::unique_ptr<Presentation> characterPresentation;
	std::unique_ptr<Cannon> characterCannon;

	std::string name{};
	float rotationAngle{ 0 };
	int desiredMoveDirectionX{ 0 };
	State desiredState{ State::Idle };

	std::vector<std::function<void(Vec2 characterWPos)>> characterMoveCallback;
	std::vector<std::function<void(float aimAngle)>> characterAimCallback;
	std::vector<std::function<void(float chargePower)>> characterChargeCallback;

	// Functions
	virtual void update(float dt) override;
	void doCharacterMoveCallback();
	void doCharacterAimCallback();
	void doCharacterChargeCallback();
};