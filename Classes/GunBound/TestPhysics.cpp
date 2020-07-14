#include "TestPhysics.h"
#include "EarCut/earcut.hpp"
#include "poly2tri/poly2tri.h"
#include <cmath>

using namespace ClipperLib;
namespace
{
	// Free Functions

	Path vecToPath(const PolyVec& vec)
	{
		Path path{};
		for (const auto v : vec)
		{
			path.push_back(IntPoint(v.x, v.y));
		}
		return path;
	}

	PolyVec pathToVec(const Path& path)
	{
		PolyVec vec{};
		for (const auto p : path)
		{
			vec.push_back(Vec2(p.X, p.Y));
		}
		return vec;
	}

	PolyVec getCircle(const Vec2& atPos, const float radius)
	{
		const int segment = 5;
		float anglePerSegmentRad = 2 * std::_Pi / segment;

		PolyVec result{};
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
	std::vector<PolyVec> getTrianglesFromPoly(const PolyVec& currentPoly,
		const std::vector<uint32_t>& triangleVertices)
	{
		std::vector<PolyVec> newTriangles{};
		newTriangles.reserve(triangleVertices.size() / 3);

		for (int i{}; i < triangleVertices.size(); i += 3)
		{
			PolyVec vert{};
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
		PolyVec vec{};
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
	PolyVec nodeSpaceCircle{};

	// Chuyển đổi các điểm từ screen space -> local space của crate
	for (const auto& vec : circle)
	{
		nodeSpaceCircle.push_back(crate->convertToNodeSpaceAR(vec));
	}

	//drawNode->drawSolidPoly(circle.data(), circle.size(), Color4F::Color4F(Color3B::GREEN, 0.2f));

	// Cho Clip trước rồi mới chuyển sang triangle;
	Clipper clipper{};
	clipper.AddPath(vecToPath(nodeSpaceCircle), PolyType::ptClip, true);

	auto collisionShapes = crate->getClippedPoly();
	for (auto& shape : collisionShapes)
	{
		clipper.AddPath(vecToPath(shape), PolyType::ptSubject, true);
	}

	Paths result;
	clipper.Execute(ClipType::ctDifference, result);

	// Convert poly sang triangle

	std::vector<PolyVec> newTriangles{};
	for (auto& path : result)
	{
		const auto vec = pathToVec(path);
		std::vector<PolyVec> clipContainer{ vec };

		std::vector<uint32_t> triangleIndices = mapbox::earcut<uint32_t>(clipContainer);
		const auto tris = getTrianglesFromPoly(vec, triangleIndices);
		newTriangles.insert(newTriangles.begin(), tris.begin(), tris.end());
	}
	crate->replaceShapes(newTriangles);
	crate->eraseArea(nodeSpaceCircle);
}

void TestPhysics::onCut2(EventMouse* ev)
{
	const auto draw = DrawNode::create();
	const auto drawAtPos = ev->getLocationInView();
	const auto circle = getCircle(drawAtPos, 30);
	PolyVec nodeSpaceCircle{};

	// Chuyển đổi các điểm từ screen space -> local space của crate
	for (const auto& vec : circle)
	{
		nodeSpaceCircle.push_back(crate->convertToNodeSpaceAR(vec));
	}

	drawNode->drawSolidPoly(circle.data(), circle.size(), Color4F::Color4F(Color3B::GREEN, 0.2f));

	const auto polyShapes = crate->getClippedPoly();
	std::vector<p2t::Point*> polyline;
	for (const auto& poly : polyShapes[0])
	{
		polyline.push_back(new p2t::Point(poly.x, poly.y));
	}

	// Hole
	std::vector<p2t::Point*> holePolyline;
	for (const auto& point : nodeSpaceCircle)
	{
		holePolyline.push_back(new p2t::Point(point.x, point.y));
	}

	// Process
	p2t::CDT cdt{ polyline };

	if (holePolyline.size() > 0)
		cdt.AddHole(holePolyline);

	cdt.Triangulate();

	std::vector<p2t::Triangle*> triangles = cdt.GetTriangles();

	std::vector<PolyVec> result{};
	for (const auto& tri : triangles)
	{
		PolyVec vec{};
		for (int i{}; i < 3; i++)
		{
			const auto p = tri->GetPoint(i);
			vec.push_back(Vec2(p->x, p->y));
		}
		result.push_back(vec);
	}
	crate->replaceShapes(result);

	// Release Memory
	for (auto& point : polyline)
	{
		delete point;
	}
	for (auto& point : holePolyline)
	{
		delete point;
	}
}
