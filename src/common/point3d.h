#pragma once
#include <cmath>
#include <string>

#include "rmath.h"
#include "rendertypes.h"

namespace rade
{
    class vector3
    {

    public:
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float alpha = 0.0f;

        float nx = 0.0f;
        float ny = 0.0f;
        float nz = 0.0f;
        bool useNormal = false;

        float u = 0.0f;
        float v = 0.0f;

        float lmU = 0.0f;
        float lmV = 0.0f;

        vector3()
        = default;

        vector3(float a, float b, float c)
        {
            Zero();
            x = a;
            y = b;
            z = c;
        }

        vector3(float a, float b, float c, float uv_a, float uv_b)
        {
            Zero();
            x = a;
            y = b;
            z = c;
            u = uv_a;
            v = uv_b;
        }

        explicit vector3(const float* vec)
        {
            Zero();
            x = vec[0];
            y = vec[1];
            z = vec[2];
        }

        // for colors
        vector3(float r, float g, float b, float a)
        {
            Zero();
            x = r;
            y = g;
            z = b;
            alpha = a;
        }

        void SetUV(float newU, float newV)
        {
            u = newU;
            v = newV;
        }

        void Scale(float s);

        void Divide(float s);

        void Zero();

        void Set(float a, float b, float c)
        {
            x = a;
            y = b;
            z = c;
        }

        void Set(vector3& p)
        {
            x = p.x;
            y = p.y;
            z = p.z;
        }

        bool IsZero() const
        {
            return ( (fabsf(x) < rade::math::cEpsilon) && (fabsf(y) < rade::math::cEpsilon) &&
                    (fabsf(z) < rade::math::cEpsilon));
        }

        void ProjectOnVector(const vector3& vector, float distance);

        vector3 Normalize();

        // check for equality
        bool operator==(const vector3& a) const;

        vector3 operator-(const vector3& v) const;

        vector3 operator+(const vector3& v) const;

        // multiplication and division by scalar
        vector3 operator*(float a) const;

        // multiplication
        vector3 operator*(const vector3& v) const;

        void Negate();

        float Dot(const vector3& p) const;

        float Distance(const vector3& p) const;

        vector3 CrossProduct(const vector3& p2) const;

        void ToFloat3(float* val) const
        {
            val[0] = x;
            val[1] = y;
            val[2] = z;
        }

        std::string ToString() const;

        void ToRenderVert(NRenderTypes::Vert* rVert) const
        {
            rVert->position.x = x;
            rVert->position.y = y;
            rVert->position.z = z;
            rVert->normal.x = nx;
            rVert->normal.y = ny;
            rVert->normal.z = nz;
            rVert->texCoord.x = u;
            rVert->texCoord.y = v;
            rVert->texCoordLM.x = lmU;
            rVert->texCoordLM.y = lmV;
        }
    };

};
