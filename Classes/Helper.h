#pragma once

#include <string_view>
#include "cocos2d.h"
#include <clipper.hpp>

USING_NS_CC;
namespace Helper
{
	
	void logVec2(const Vec2& point);
	Vec2 rotatePointAroundOrigin(const Vec2 point, const float angle);
	Vec2 rotateOffsetArountPoint(Vec2 point, const Vec2 offset, const float angle);

	/// <summary>
	/// Tạo ra tọa độ hình tròn
	/// </summary>
	/// <param name="atPos">tại điểm</param>
	/// <param name="radius">bán kính</param>
	/// <returns>Danh sách các điểm cần vẽ</returns>
	std::vector<Vec2> getCircle(const Vec2& atPos, const float radius);

	/// <summary>
	/// Hàm này chỉ là 1 hàm gói gọn lại những dòng lệnh xác định shapeA hoặc shapeB là gì
	/// </summary>
	/// <param name="contact">Physics Contact khi có va chạm xảy ra</param>
	/// <param name="self">Object có Category Mask trùng khớp với selfMask</param>
	/// <param name="other">Object còn lại</param>
	/// <param name="selfMask">Category Mask cần tìm</param>
	/// <returns></returns>
	bool detectWhichCollider(const PhysicsContact & contact, PhysicsShape* self, PhysicsShape* other, int selfMask);
}

namespace ProjectileMotion2D
{
	/// <summary>
	/// Tính toán unit vector từ một góc theo phương Ox (xoay ngược chiều kim đồng hồ)
	/// </summary>
	/// <param name="angle">Góc bắn. Góc dương sẽ xoay ngược chiều, góc âm sẽ xoay cùng chiều kim đồng hồ</param>
	/// <returns>Vector đơn vị</returns>
	Vec2 calc_unitVector(float angle);

	/// <summary>
	/// Convert từ Tốc độ "khởi điểm" (pixel/s) sang Vận tốc "khởi điểm" (x pixel/s, y pixel/s)
	/// </summary>
	/// <param name="initSpeed">Tốc độ "khởi điểm" cần chuyển đổi</param>
	/// <param name="angle">Góc bắn</param>
	Vec2 convert_speedToVelocity(float initSpeed, float angle);

	/// <summary>
	/// Convert từ Vận tốc "khởi điểm" (x pixel/s, y pixel/s) sang Tốc độ (pixel/s)
	/// </summary>
	/// <param name="initVelocity">Vận tốc "khởi điểm"</param>
	float convert_velocityToSpeed(const Vec2& initVelocity);

	/// <summary>
	/// Giải phương trình bậc 2 : Ax^2 + Bx + C = 0;
	/// </summary>
	/// <returns>Nghiệm trả về có 2 giá trị. Trả về <0, 0> nếu giải ko được</returns>
	std::pair<float, float> solve_quadrantEquation(float A, float B, float C);

	/// <summary>
	/// Tìm thời gian mà vật rơi xuống đất (T). Biết trước: cao độ (d_oy) và vận tốc ban đầu (v_oy)
	/// </summary>
	/// <param name="d_oy">Nếu bắn từ trên xuống thì d_oy sẽ dương. Nếu bắn từ dưới lên thì d_oy sẽ âm</param>
	/// <param name="v_oy">Vận tốc khởi điểm theo phương Oy</param>
	/// <param name="a_y">Gia tốc theo phương Oy</param>
	/// <returns></returns>
	float find_t(float d_oy, float v_oy, float a_y);

	/// <summary>
	/// Tìm thời gian mà vật rơi xuống đất (T). Biết trước: displacement (d_o) và vận tốc ban đầu(v_o)
	/// </summary>
	/// <param name="d_o">Displacement khởi điểm. Nếu bắn từ trên xuống thì d_o.y sẽ dương. Nếu bắn từ dưới lên thì d_o.y sẽ âm</param>
	/// <param name="v_o">Vận tốc khởi điểm</param>
	/// <param name="a">Gia tốc</param>
	/// <returns></returns>
	float find_t(const Vec2& d_o, const Vec2& v_o, const Vec2& a);

	/// <summary>
	/// Tìm displacement theo một phương nhất định (Ox hoặc Oy) là bao nhiêu tại thời điểm t
	/// </summary>
	/// <param name="v_o">Vận tốc khởi điểm theo phương nhất định</param>
	/// <param name="a">Gia tốc theo phương nhất định</param>
	/// <param name="t">Thời điểm t</param>
	/// <param name="d_o">Displacement khởi điểm theo phương nhất định (nếu có)</param>
	/// <returns></returns>
	float find_d(float v_o, float a, float t, float d_o);

	/// <summary>
	/// Tìm displacement theo cả 2 phương là bao nhiêu tại thời điểm t
	/// </summary>
	/// <param name="v_o">Vận tốc khởi điểm</param>
	/// <param name="a">Gia tốc</param>
	/// <param name="t">Thời điểm t</param>
	/// <param name="d_o">Displacement khởi điểm (nếu có)</param>
	/// <returns></returns>
	Vec2 find_d(const Vec2& v_o, const Vec2& a, float t, const Vec2& d_o);
}

namespace ClipperLib
{
	/// <summary>
	/// Chuyển đổi các điểm Vec2 thành các điểm IntPoints để sử dụng cho thư viện clipper
	/// </summary>
	Path vecToPath(const std::vector<Vec2>& points);

	/// <summary>
	/// Chuyển đổi Path do clipper xuất ra thành các điểm Vec2
	/// </summary>
	std::vector<Vec2> pathToVec(const Path& path);
}

namespace mapbox
{
	/// <summary>
	/// Hàm này trả về list các tam giác được tính toán từ 1 list các điểm rời rạc của 1 polygon.
	/// </summary>
	/// <param name="polyline">List các điểm rời rạc của 1 polygon</param>
	/// <returns>Trả về list của: 1 list 3 đỉnh tam giác</returns>
	std::vector<std::vector<Vec2>> getTrianglesFromPolyline(const std::vector<Vec2>& polyline);

	/// <summary>
	/// Hàm này trả về list các tam giác được lấy ra từ 1 nhóm các điểm cho trước.
	/// </summary>
	/// <param name="currentPoly">Nhóm các điểm cần lọc ra</param>
	/// <param name="triangleVertices">Nhóm các đỉnh tam giác</param>
	/// <returns></returns>
	std::vector<std::vector<Vec2>> getTrianglesFromPoly(const std::vector<Vec2>& currentPoly, const std::vector<uint32_t>& triangleVertices);
}

#ifndef RETURN_FALSE_IF_NULL_PTR
#define RETURN_FALSE_IF_NULL_PTR(p, message) do{if (!p){CCLOGERROR(message); return false;}}while(0)
#endif // !RETURN_FALSE_IF_NULL_PTR
