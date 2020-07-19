#pragma once

#include "cocos2d.h"
#include "SpritePhysics.h"
#include "Character.h"

USING_NS_CC;

constexpr int COLLISION_MASK_NONE = 0;
constexpr int COLLISION_MASK_CHARACTER = 1 << 0;
constexpr int COLLISION_MASK_SPRITE_PHYSICS = 1 << 1;
constexpr int COLLISION_MASK_AMMO = 1 << 2;

// Class này sẽ đảm nhiệm việc setup mọi thứ lên scene
class GunBoundScene : public Scene
{
public:
	static Scene* createScene();
	virtual bool init() override;
	CREATE_FUNC(GunBoundScene);

	static Vec2 getAcceleration();

private:
	// Tất cả những thứ sẽ setup gồm:
	std::vector<SpritePhysics*> allSpritePhysics{};

	// Background (2 cái)
	Sprite* backgroundLayer1{};
	Sprite* backgroundLayer2{};

	// Terrain (1 cái) kèm physic body
	CC_SYNTHESIZE_READONLY(SpritePhysics*, terrain, SpriteTerrain);

	// Tree (1 cái) kèm physic body
	CC_SYNTHESIZE_READONLY(SpritePhysics*, tree, SpriteTree);

	// Player (1 cái) kèm physic body
	CC_SYNTHESIZE(Character*, player, Player);

	// Các thuộc tính:
	inline static Vec2 acceleration{};

	void addToScene(SpritePhysics* sprite, Vec2&& atPos);

	bool physicsCollisionFilterRules(PhysicsContact& contact);

};



/*
Vấn đề bây giờ là tính toán thế nào để việc đạn bay và di chuyển nhân vật trở nên hợp lý
Kích thước  resolution bây giờ đang là 1280x720.
Cứ cho rằng map lớn nhất là gấp 3 theo mỗi chiều.
Vậy max width = 3840
Vậy max height = 2160

Bây giờ hợp lý nhất là tính tốc độ di chuyển của nhân vật bao nhiêu?
Thử ước tính vận tốc sang pixel/s
Kích thước nhân vật hiện tại là 150x150 pixels
So với tỉ lệ màn hình (1280x720) là bằng 10% chiều dài và 20% chiều cao.

Bây giờ thử thấy số liệu thật đi
Một con người trung bình chạy được 24km/h -> 0.4km/ph -> 6m/s với chiều cao cho là 1.7m
=> Vậy tính ra con người chạy tốc độ bằng gấp 3.5 lần đơn vị chiều cao của mình / giây
Trong trường hợp Character cao 150px thì tốc độ chạy tạm cho là 450px/s -> khoảng 2.8s là sẽ chạy hết quãng đường 1280px

1. Quy đổi đơn vị kích thước
Tạm lấy tỉ lệ như Unity: 100 px = 1m
Công thức tính m->px = m * 100
Vậy với Character cao 150px = 1.5m

2. Gia tốc rơi:
- Gia tốc rơi trung bình là 9.81 m/s/s => 981 px/s/s

3. Tốc độ di chuyển của Character:
- Tạm cho gấp 3 lần chiều cao: => 450 px/s

4. Tốc độ đạn bay:
- Tạm cho nó bay nhanh gấp 5 gần tốc độ di chuyển của Character: => 2250 px/s
*/

