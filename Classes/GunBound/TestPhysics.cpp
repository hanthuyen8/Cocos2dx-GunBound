#include "TestPhysics.h"
#include "EarCut/earcut.hpp"
#include <cmath>

using namespace ClipperLib;
namespace
{
	// Free Functions

	Path vecToPath(const std::vector<Vec2>& vec)
	{
		Path path{};
		for (const auto v : vec)
		{
			path.push_back(IntPoint(v.x, v.y));
		}
		return path;
	}

	std::vector<Vec2> pathToVec(const Path& path)
	{
		std::vector<Vec2> vec{};
		for (const auto p : path)
		{
			vec.push_back(Vec2( p.X, p.Y ));
		}
		return vec;
	}

	std::vector<Vec2> getCircle(const Vec2& atPos, const float radius)
	{
		const int segment = 5;
		float anglePerSegmentRad = 2 * std::_Pi / segment;

		std::vector<Vec2> result{};
		result.reserve(segment);

		for (int i{}; i < segment; i++)
		{
			Vec2 unit{ std::cos(anglePerSegmentRad * i), std::sin(anglePerSegmentRad * i) };
			result.push_back(atPos + (unit * radius));
		}

		return result;
	}

	/// <summary>
	/// Hàm này trả về list các tam giác được lấy ra từ 1 nhóm các điểm cho trước.
	/// </summary>
	/// <param name="currentPoly">Nhóm các điểm cần lọc ra</param>
	/// <param name="triangleVertices">Nhóm các đỉnh tam giác</param>
	/// <returns></returns>
	std::vector<std::vector<Vec2>> getTrianglesFromPoly(const std::vector<Vec2>& currentPoly,
		const std::vector<uint32_t>& triangleVertices)
	{
		std::vector<std::vector<Vec2>> newTriangles{};
		newTriangles.reserve(triangleVertices.size() / 3);

		for (int i{}; i < triangleVertices.size(); i += 3)
		{
			std::vector<Vec2> vert{};
			vert.reserve(3);
			for (int k{}; k < 3; k++)
			{
				auto index = triangleVertices[i + k];
				vert.push_back(currentPoly[index]);
			}
			newTriangles.push_back(vert);
		}
		return newTriangles;
	}

	void drawNodeToScreen(DrawNode* draw, Path& path, Color4F color)
	{
		std::vector<Vec2> vec{};
		for (const auto point : path)
		{
			vec.push_back(Vec2(point.X, point.Y));
		}
		draw->drawSolidPoly(vec.data(), vec.size(), color);
	}
}

Scene* TestPhysics::createScene()
{
	return TestPhysics::create();
}

bool TestPhysics::init()
{
	if (!Scene::initWithPhysics())
		return false;

	// Turn on Physics Debugger
	this->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	this->setPhysics3DDebugCamera(Camera::getDefaultCamera());

	crate = Crate::createCrate("GunBound/Crate.png");
	crate->setPosition(400, 350);
	this->addChild(crate);

	drawNode = DrawNode::create();
	this->addChild(drawNode);

	// Click and deform crate
	const auto mouseEvListener = EventListenerMouse::create();
	mouseEvListener->onMouseDown = CC_CALLBACK_1(TestPhysics::onCut, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseEvListener, this);

	return true;
}

void TestPhysics::onCut(EventMouse* ev)
{
	const auto draw = DrawNode::create();
	const auto drawAtPos = ev->getLocationInView();
	const auto circle = getCircle(drawAtPos, 30);
	std::vector<Vec2> nodeSpaceCircle{};

	// Chuyển đổi các điểm từ screen space -> local space của crate
	for (const auto& vec : circle)
	{
		nodeSpaceCircle.push_back(crate->convertToNodeSpaceAR(vec));
	}

	drawNode->drawSolidPoly(circle.data(), circle.size(), Color4F::Color4F(Color3B::GREEN, 0.2f));

	// Cho Clip trước rồi mới chuyển sang triangle;
	Clipper clipper{};
	clipper.AddPath(vecToPath(nodeSpaceCircle), PolyType::ptClip, true);

	auto collisionShapes = crate->getCollisionPoints(circle);
	for (auto& shape : collisionShapes)
	{
		clipper.AddPath(vecToPath(shape), PolyType::ptSubject, true);
	}

	Paths result;
	clipper.Execute(ClipType::ctDifference, result);

	// Convert poly sang triangle
	for (auto& path : result)
	{
		const auto vec = pathToVec(path);
		std::vector<std::vector<Vec2>> clipContainer{ vec };

		std::vector<uint32_t> triangleIndices = mapbox::earcut<uint32_t>(clipContainer);

		const auto newTriangles = getTrianglesFromPoly(vec, triangleIndices);
		crate->addShapes(newTriangles);
	}

	// Đây là list các tam giác có va chạm với circle
	// Ta phải cho lần lượt các tam giác đó clip với circle
	// Sau khi clip thì sẽ sinh ra nhiều tam giác nữa. Add lần lượt các tam giác đó vào lại crate.

	// Mình đang làm không đúng
	// Không đúng ở chỗ khi đã tách 1 poly thành nhiều tris. Thì phải lưu lại tất cả các tris đó.
	// Sau khi click thì lấy ra các tris nào va chạm với các điểm mới.
	// Đồng thời remove các tris đó ra khỏi physicsBody.

	// Từ các tris có va chạm thì clip nó với các điểm mới để tạo ra các tris mới khác.
	// Thêm các tris mới đó vào physicBody.

	// Việc lưu trữ các tris nhỏ nhỏ và lấy ra cái nào contains Point để xử lý có vẻ không ổn
	// Bây giờ thử việc xóa hết tất cả các physicsShape rồi generate lại từ đầu
	// Class Crate sẽ không chứa từng tris nhỏ nữa mà chứa một poly lớn luôn (Paths)
}
