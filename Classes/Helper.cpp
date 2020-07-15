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
}