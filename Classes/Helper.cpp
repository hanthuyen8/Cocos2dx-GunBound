#include <vector>
#include "Helper.h"
#include "EarCut/earcut.hpp"

namespace Helper
{
	/// <summary>
		/// Hàm này trả về list các tam giác được lấy ra từ 1 nhóm các điểm cho trước.
		/// </summary>
		/// <param name="polyline">Nhóm các điểm cần lọc ra</param>
		/// <param name="triangleIndices">Nhóm các đỉnh tam giác</param>
		/// <returns>Trả về 1 list của: 1 list 3 đỉnh tam giác</returns>
	std::vector<std::vector<Vec2>> getTrianglesFromPoly(const std::vector<Vec2>& polyline, const std::vector<uint32_t>& triangleIndices)
	{
		std::vector<std::vector<Vec2>> newTriangles{};
		newTriangles.reserve(triangleIndices.size() / 3);

		for (int i{}; i < triangleIndices.size(); i += 3)
		{
			std::vector<Vec2> vert{};
			vert.reserve(3);
			for (int k{}; k < 3; k++)
			{
				auto index = triangleIndices[i + k];
				vert.push_back(polyline[index]);
			}
			newTriangles.push_back(vert);
		}
		return newTriangles;
	}

	/// <summary>
	/// Hàm này trả về list các tam giác được tính toán từ 1 list các điểm rời rạc của 1 polygon.
	/// </summary>
	/// <param name="polyline">List các điểm rời rạc của 1 polygon</param>
	/// <returns>Trả về list của: 1 list 3 đỉnh tam giác</returns>
	std::vector<std::vector<Vec2>> getTrianglesFromPolyline(const std::vector<Vec2>& polyline)
	{
		return getTrianglesFromPoly(polyline, mapbox::earcut(std::vector<std::vector<Vec2>> {polyline}));
	}

	void logVec2(const Vec2& point)
	{
		cocos2d::log("Vector: (%f, %f)", point.x, point.y);
	}

	Vec2 rotatePointAroundOrigin(const Vec2 point, const float angle)
	{
		// Công thức counter-clockwise rotation:
		// x2 = cosA * x1 − sinA * y1
		// y2 = sinA * x1 + cosA * y1

		// Công thức clockwise rotation:
		// x2 = cosA * x1 + sinA * y1
		// y2 = -sinA * x1 + cosA * y1

		const auto rad{ CC_DEGREES_TO_RADIANS(-angle) };
		const float cosA = std::cos(rad);
		const float sinA = std::sin(rad);
		Vec2 output{ 0,0 };
		output.x = cosA * point.x - sinA * point.y;
		output.y = sinA * point.x + cosA * point.y;
		return output;
	}

	Vec2 rotateOffsetArountPoint(Vec2 point, const Vec2 offset, const float angle)
	{
		const auto rad{ CC_DEGREES_TO_RADIANS(angle) };
		const float cosA = std::cos(rad);
		const float sinA = std::sin(rad);

		// Translate Point back to Origin
		point -= offset;

		Vec2 output{};
		output.x = cosA * point.x - sinA * point.y;
		output.y = sinA * point.x + cosA * point.y;

		// Translate Point back to Offet
		point += output + offset;
		return point;
	}
}