#pragma once

#include <cmath>
#include <cstdint>

class CPoint3D;

namespace RMATH
{
	const float cPi = 3.14159265358979323846f;
	const float c2Pi = cPi * 2.0f;
	const float cPiOver2 = cPi / 2.0f;
	const float c1OverPi = 1.0f / cPi;
	const float c1Over2Pi = 1.0f / c2Pi;
	const float cPiOver180 = cPi / 180.0f;
	const float c180OverPi = 180.0f / cPi;
	const float cEpsilonLarger  = 0.02f;
	const float cEpsilon  = 0.001f;
	const float cEpsilonSmaller = 0.00001f;

	enum ESide
	{
		ESide_FRONT = 0,
		ESide_BACK,
		ESide_SPAN,
		ESide_ON
	};

	// convert between "field of view" and "zoom"
	inline float FovToZoom(float fov) { return 1.0f / tan(fov * .5f); }
	inline float ZoomToFov(float zoom) { return 2.0f * atan(1.0f / zoom); }
	inline float DegToRad(float deg) { return deg * cPiOver180; }
	inline float RadToDeg(float rad) { return rad * c180OverPi; }
	inline float vsqr(float x) { return x*x; };

	inline float RandomFloat(float a, float b)
	{
		return ((b - a) * ((float)rand() / (float)RAND_MAX)) + a;
	}

};
