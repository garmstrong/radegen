#include <sstream>
#include "point3d.h"

void CPoint3D::Scale(float s)
{
	x *= s;
	y *= s;
	z *= s;
}

void CPoint3D::Divide(float s)
{
	x /= s;
	y /= s;
	z /= s;
}

void CPoint3D::Zero()
{
	x = 0;
	y = 0;
	z = 0;
	u = 0;
	v = 0;
	//lmu = 0;
	//lmv = 0;
	nx = 0;
	ny = 0;
	nz = 0;
	useNormal = false;
}

void CPoint3D::ProjectOnVector(const CPoint3D& vector, const float distance)
{
	// project a point from the camera in the view dir
	x = (vector.x * distance) + x;
	y = (vector.y * distance) + y;
	z = (vector.z * distance) + z;
}

CPoint3D CPoint3D::Normalize()
{
	float length;
	length = x * x + y * y + z * z;
	length = static_cast<float>(sqrt(length));
	if (length > 0.0f)
	{
		float ilength = 1 / length;
		x *= ilength;
		y *= ilength;
		z *= ilength;
	}
	return *this;
}

// check for equality
bool CPoint3D::operator==(const CPoint3D& a) const
{
	return (fabs(x - a.x) < RMATH::cEpsilon) && (fabs(y - a.y) < RMATH::cEpsilon) && (fabs(z - a.z) < RMATH::cEpsilon);
}

CPoint3D CPoint3D::operator-(const CPoint3D& v) const
{
	CPoint3D newPos;
	newPos = *this;
	newPos.Set(x - v.x,
			y - v.y,
			z - v.z);
	return newPos;
}

CPoint3D CPoint3D::operator+(const CPoint3D& v) const
{
	CPoint3D newPos;
	newPos = *this;
	newPos.Set(x + v.x,
			y + v.y,
			z + v.z);
	return newPos;
}

// multiplication and division by scalar
CPoint3D CPoint3D::operator*(const float a) const
{
	return CPoint3D(x * a,
			y * a,
			z * a);
}

// multiplication
CPoint3D CPoint3D::operator*(const CPoint3D& v) const
{
	return CPoint3D(x * v.x,
			y * v.y,
			z * v.z);
}

void CPoint3D::Negate()
{
	x = -x;
	y = -y;
	z = -z;
}

float CPoint3D::Dot(const CPoint3D& p) const
{
	return x * p.x + y * p.y + z * p.z;
}

float CPoint3D::Distance(const CPoint3D& p) const
{
	return sqrt(RMATH::vsqr(fabs(p.x - x)) + RMATH::vsqr(fabs(p.y - y)) + RMATH::vsqr(fabs(p.z - z)));
	/*CPoint3D r;
	r.x = x - p.x;
	r.y = y - p.y;
	r.z = z - p.z;
	return sqrt(r.x*r.x + r.y*r.y + r.z*r.z);*/
}

CPoint3D CPoint3D::CrossProduct(const CPoint3D& p2) const
{
	return CPoint3D(
			y * p2.z - z * p2.y,
			z * p2.x - x * p2.z,
			x * p2.y - y * p2.x);
}

std::string CPoint3D::ToString() const
{
	std::ostringstream pointString;
	pointString << "(" << x << ", " << y << ", " << z << ")";
	return pointString.str();
}
