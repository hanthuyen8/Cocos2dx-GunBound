#pragma once

#include <string_view>
#include "cocos2d.h"

namespace Helper
{
	USING_NS_CC;
	/// <summary>
		/// Hàm này trả về list các tam giác được lấy ra từ 1 nhóm các điểm cho trước.
		/// </summary>
		/// <param name="polyline">Nhóm các điểm cần lọc ra</param>
		/// <param name="triangleIndices">Nhóm các đỉnh tam giác</param>
		/// <returns>Trả về 1 list của: 1 list 3 đỉnh tam giác</returns>
	std::vector<std::vector<Vec2>> getTrianglesFromPoly(const std::vector<Vec2>& polyline, const std::vector<uint32_t>& triangleIndices);

	/// <summary>
	/// Hàm này trả về list các tam giác được tính toán từ 1 list các điểm rời rạc của 1 polygon.
	/// </summary>
	/// <param name="polyline">List các điểm rời rạc của 1 polygon</param>
	/// <returns>Trả về list của: 1 list 3 đỉnh tam giác</returns>
	std::vector<std::vector<Vec2>> getTrianglesFromPolyline(const std::vector<Vec2>& polyline);

	void logVec2(const Vec2& point);

	Vec2 rotatePointAroundOrigin(const Vec2 point, const float angle);
	Vec2 rotateOffsetArountPoint(Vec2 point, const Vec2 offset, const float angle);
	std::vector<Vec2> getCircle(const Vec2& atPos, const float radius);
}

#ifndef RETURN_FALSE_IF_NULL_PTR
#define RETURN_FALSE_IF_NULL_PTR(p, message) do{if (!p){CCLOGERROR(message); return false;}}while(0)
#endif // !RETURN_FALSE_IF_NULL_PTR
