#pragma once

#include <cmath>
#include "point3d.h"

class CPoint2D
{

public:
	float x;
	float y;
	float z;


	CPoint2D()
	{
		Zero();
	}

	CPoint2D(float a, float b)
	{
		Zero();
		x = a;
		y = b;
	}

	virtual ~CPoint2D()
	{
	};


	inline void Scale(float s)
	{
		x *= s;
		y *= s;
	}

	void Zero()
	{
		x = 0;
		y = 0;
	}

	inline void Set(float a, float b)
	{
		x = a;
		y = b;
	}

	inline void Set(const CPoint2D& p)
	{
		x = p.x;
		y = p.y;
	}

	inline bool IsZero()
	{
		return (x == 0 && y == 0);
	}

	inline void ProjectOnVector(const CPoint3D& vector, const float distance)
	{
		// project a point from the camera in the view dir
		x = (vector.x * distance) + x;
		y = (vector.y * distance) + y;
	}

	inline void Normalize()
	{
		float length = x * x + y * y;
		length = (float)sqrt(length);
		if (length)
		{
			float ilength = 1 / length;
			x *= ilength;
			y *= ilength;
		}
	}

	// check for equality
	inline bool operator==(const CPoint2D& a) const
	{
		return (fabs(x - a.x) < RMATH::cEpsilon) && (fabs(y - a.y) < RMATH::cEpsilon);
	}

	inline CPoint2D operator-(const CPoint2D& v) const
	{
		CPoint2D newPos;
		newPos = *this;
		newPos.Set(x - v.x,
				y - v.y);
		return newPos;
	}

	inline CPoint2D operator+(const CPoint2D& v) const
	{
		CPoint2D newPos;
		newPos = *this;
		newPos.Set(x + v.x,
				y + v.y);
		return newPos;
	}

	// multiplication and division by scalar
	inline CPoint2D operator*(float a) const
	{
		return CPoint2D(x * a,
				y * a);
	}

	// multiplication
	inline CPoint2D operator*(CPoint2D& v) const
	{
		return CPoint2D(x * v.x,
				y * v.y);
	}

	inline void Negate()
	{
		x = -x;
		y = -y;
	}

	inline float DotProduct(const CPoint2D& p) const
	{
		return x * p.x + y * p.y;
	}

	inline float Distance(const CPoint2D& p) const
	{
		return sqrt(RMATH::vsqr(fabs(p.x - x)) + RMATH::vsqr(fabs(p.y - y)));
	}

	inline CPoint2D CrossProduct(const CPoint2D& p2)
	{
		return CPoint2D(y * p2.z - z * p2.y,
				z * p2.x - x * p2.z);
	}
};
