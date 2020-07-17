#pragma once

#include "cocos2d.h"
#include "Cannon.h"
#include <string_view>
#include "IDamageable.h"

USING_NS_CC;

// Physics của Character sẽ như sau:
/*
	Bước 1: Tìm khoảng cách
	Mỗi vòng update Character sẽ bắn ra 2 raycast xuống mặt đất để xác định khoảng cách với mặt đất.
	2 Raycast đó bắt đầu từ tâm đi chéo về 2 hướng: phía trước mặt và phía sau lưng.
	Góc bắn sẽ là +-30 độ so với hướng vector_down.
	Khoảng cách bắn sẽ là đường chéo của hình vuông : sqrt(2*radius*radius)
	1. Nếu bắn không trúng gì cả: tức là hoàn toàn trên không trung. Cho nhích player xuống bằng : khoảng cách bắn - radius
	2. Nếu bắn chỉ trúng 1 trong 2 điểm: kệ nó
	3. Nếu bắn trúng cả 2: tức là Character đã yên vị trên mặt đất -> chuyển đến Bước 2

	Bước 2: Thăng bằng Character
	Lấy 2 điểm vừa bắn được và kiểm tra nó có song song với trục x hay không?
	1. song song: thì cho Character đứng vuông góc với mặt đất luôn (rotation = 0);
	2. ko song song: thì cho Character vuông góc với hướng lên của mặt đất (rotation = rotation của mặt đất)
	-> Vậy tóm gọn lại là gán luôn angle của mặt đất cho Character

*/
class Character : public Sprite, public IDamageable
{
public:
	static inline int COLLISION_CATEGORY{ 0x00 };
	static inline int COLLISION_WITH{ 0x00 };

	static Character* create(std::string_view fileName, float radius);
	bool init(std::string_view fileName, float radius);

	// Character sẽ có các thuộc tính:
	CC_SYNTHESIZE(float, moveSpeed, MoveSpeed);

	// Character sẽ có các hàm di chuyển:
	void listenToKeyboardMovement();
	virtual void setDamage(const std::vector<Vec2>& damagedPoints) override;

private:
	CC_SYNTHESIZE_READONLY(PhysicsBody*, physicsBody, PhysicsBody);

	float radius{};
	float moveHorizontal{};
	Vec2 groundNormal{ Vec2::ZERO };
	float fallSpeed{ 10000 };
	bool isKeyPressed{ false };
	bool isFacingToLeft{ false };
	bool isFireAndStopMoving{ false };
	PhysicsShape* currentCollider{};
	Cannon* cannon{};

	virtual void update(float dt) override;
	void onKeyPressed(EventKeyboard::KeyCode key, Event*);
	float checkGravity();
};

