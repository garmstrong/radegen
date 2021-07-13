#pragma once

#include <cmath>
#include "point3d.h"

namespace rade
{
    class vector2
    {

    public:
        float x = 0.0f;
        float y = 0.0f;
        //float z = 0.0f;

        vector2()
        {
            Zero();
        }

        vector2(float a, float b)
        {
            Zero();
            x = a;
            y = b;
        }

        void Scale(float s)
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

        inline void Set(const vector2& p)
        {
            x = p.x;
            y = p.y;
        }

        bool IsZero()
        {
            return ( (fabsf(x) < rade::math::cEpsilon) && (fabsf(y) < rade::math::cEpsilon) );
        }

        void ProjectOnVector(const rade::vector3& vector, const float distance)
        {
            // project a point from the camera in the view dir
            x = (vector.x * distance) + x;
            y = (vector.y * distance) + y;
        }

        void Normalize()
        {
            float length = x * x + y * y;
            length = (float)sqrtf(length);
            if (length > 0.0f)
            {
                float ilength = 1 / length;
                x *= ilength;
                y *= ilength;
            }
        }

        // check for equality
        bool operator==(const vector2& a) const
        {
            return (fabsf(x - a.x) < rade::math::cEpsilon) && (fabsf(y - a.y) < rade::math::cEpsilon);
        }

        vector2 operator-(const vector2& v) const
        {
            vector2 newPos;
            newPos = *this;
            newPos.Set(x - v.x,
                    y - v.y);
            return newPos;
        }

        vector2 operator+(const vector2& v) const
        {
            vector2 newPos;
            newPos = *this;
            newPos.Set(x + v.x,
                    y + v.y);
            return newPos;
        }

        // multiplication and division by scalar
        vector2 operator*(float a) const
        {
            return {x * a,
                    y * a};
        }

        // multiplication
        vector2 operator*(vector2& v) const
        {
            return {x * v.x,
                    y * v.y};
        }

        void Negate()
        {
            x = -x;
            y = -y;
        }

        float DotProduct(const vector2& p) const
        {
            return x * p.x + y * p.y;
        }

        float Distance(const vector2& p) const
        {
            return sqrtf(rade::math::vsqr(fabsf(p.x - x)) + rade::math::vsqr(fabsf(p.y - y)));
        }

//        vector2 CrossProduct(const vector2& p2) const
//        {
//            return {y * p2.z - z * p2.y,
//                    z * p2.x - x * p2.z};
//        }
    };
};
