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

	/// <summary>
	/// Tạo ra tọa độ hình tròn
	/// </summary>
	/// <param name="atPos">tại điểm</param>
	/// <param name="radius">bán kính</param>
	/// <returns>Danh sách các điểm cần vẽ</returns>
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
}

namespace ProjectileMotion2D
{
	Vec2 calc_unitVector(float angle)
	{
		const auto rad{ CC_DEGREES_TO_RADIANS(angle) };
		return Vec2{ std::cos(rad), std::sin(rad) };
	}

	/// <summary>
	/// Convert từ Tốc độ "khởi điểm" (pixel/s) sang Vận tốc "khởi điểm" (x pixel/s, y pixel/s)
	/// </summary>
	/// <param name="initSpeed">Tốc độ "khởi điểm" cần chuyển đổi</param>
	/// <param name="angle">Góc bắn</param>
	Vec2 convert_speedToVelocity(float initSpeed, float angle)
	{
		const auto rad{ CC_DEGREES_TO_RADIANS(angle) };
		return Vec2{ initSpeed * std::cos(rad), (initSpeed * std::sin(rad)) };
	}

	/// <summary>
	/// Convert từ Vận tốc "khởi điểm" (x pixel/s, y pixel/s) sang Tốc độ (pixel/s)
	/// </summary>
	/// <param name="initVelocity">Vận tốc "khởi điểm"</param>
	float convert_velocityToSpeed(const Vec2& initVelocity)
	{
		return initVelocity.length();
	}

	/// <summary>
	/// Giải phương trình bậc 2 : Ax^2 + Bx + C = 0;
	/// </summary>
	/// <returns>Nghiệm trả về có 2 giá trị. Trả về <0, 0> nếu giải ko được</returns>
	std::pair<float, float> solve_quadrantEquation(float A, float B, float C)
	{
		const auto delta{ std::sqrt(std::pow(B, 2) - (4 * A * C)) };
		// Nếu delta > 0: phương trình 2 nghiệm
		// Nếu delta == 0: phương trình chỉ có 1 nghiệm
		// Nếu delta < 0: phương trình số phức (xem như không có nghiệm)

		if (delta < 0)
			return std::make_pair(0.0f, 0.0f);

		const auto result1 = (-B + delta) / (2 * A);
		const auto result2 = (-B - delta) / (2 * A);

		return std::make_pair(result1, result2);
	}

	/// <summary>
	/// Tìm thời gian mà vật rơi xuống đất (T). Biết trước: cao độ (d_oy) và vận tốc ban đầu (v_oy)
	/// </summary>
	/// <param name="d_oy">Nếu bắn từ trên xuống thì d_oy sẽ dương. Nếu bắn từ dưới lên thì d_oy sẽ âm</param>
	/// <param name="v_oy">Vận tốc khởi điểm theo phương Oy</param>
	/// <param name="a_y">Gia tốc theo phương Oy</param>
	/// <returns></returns>
	float find_t(float d_oy, float v_oy, float a_y = -9.81f)
	{
		// Phương trình Công thức (phương trình bậc 2):
		// H + V.y * T - (0.5 * g * T^2) = 0
		// đưa về phương trình: Ax^2 + Bx + C = 0 để giải: (-0.5 * g)T^2 + (V.y)T + H = 0;
		// Kết quả: x = (-B +- sqrt(B^2 - 4AC)) / 2A

		const float A = 0.5f * a_y;
		const float B = v_oy;
		const float C = d_oy;

		const auto result{ solve_quadrantEquation(A, B, C) };

		// Vì thời gian là đại lượng không thể âm. Cho nên chỉ lấy ra giá trị nào không âm
		return MAX(result.first, result.second);
	}

	/// <summary>
	/// Tìm thời gian mà vật rơi xuống đất (T). Biết trước: displacement (d_o) và vận tốc ban đầu(v_o)
	/// </summary>
	/// <param name="d_o">Displacement khởi điểm. Nếu bắn từ trên xuống thì d_o.y sẽ dương. Nếu bắn từ dưới lên thì d_o.y sẽ âm</param>
	/// <param name="v_o">Vận tốc khởi điểm</param>
	/// <param name="a">Gia tốc</param>
	/// <returns></returns>
	float find_t(const Vec2& d_o, const Vec2& v_o, const Vec2& a)
	{
		return find_t(d_o.y, v_o.y, a.y);
	}

	/// <summary>
	/// Tìm displacement theo một phương nhất định (Ox hoặc Oy) là bao nhiêu tại thời điểm t
	/// </summary>
	/// <param name="v_o">Vận tốc khởi điểm theo phương nhất định</param>
	/// <param name="a">Gia tốc theo phương nhất định</param>
	/// <param name="t">Thời điểm t</param>
	/// <param name="d_o">Displacement khởi điểm theo phương nhất định (nếu có)</param>
	/// <returns></returns>
	float find_d(float v_o, float a, float t, float d_o = 0)
	{
		return (0.5f * a * std::pow(t, 2)) + (v_o * t) + d_o;
	}

	/// <summary>
	/// Tìm displacement theo cả 2 phương là bao nhiêu tại thời điểm t
	/// </summary>
	/// <param name="v_o">Vận tốc khởi điểm</param>
	/// <param name="a">Gia tốc</param>
	/// <param name="t">Thời điểm t</param>
	/// <param name="d_o">Displacement khởi điểm (nếu có)</param>
	/// <returns></returns>
	Vec2 find_d(const Vec2& v_o, const Vec2& a, float t, const Vec2& d_o = Vec2::ZERO)
	{
		const auto d_x{ find_d(v_o.x, a.x, t, d_o.x) };
		const auto d_y{ find_d(v_o.y, a.y, t, d_o.y) };

		return Vec2{ d_x, d_y };
	}
}

namespace ClipperLib
{
	Path vecToPath(const std::vector<Vec2>& points)
	{
		Path path{};
		for (const auto point : points)
		{
			path.push_back(IntPoint(point.x, point.y));
		}
		return path;
	}

	std::vector<Vec2> pathToVec(const Path& path)
	{
		std::vector<Vec2> points{};
		for (const auto point : path)
		{
			points.push_back(Vec2(point.X, point.Y));
		}
		return points;
	}
}

namespace mapbox
{
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
}
