#pragma once

#include <cmath>
#include <string>
#include "rmath.h"

class CPoint3D
{

public:
    float x;
    float y;
    float z;
    float alpha;

    float nx;
    float ny;
    float nz;
    bool useNormal;

    float u;
    float v;

    float lmU;
    float lmV;

//	float lmu;
//	float lmv;

    CPoint3D()
    {
        Zero();
    }

    CPoint3D(float a, float b, float c)
    {
        Zero();
        x = a;
        y = b;
        z = c;
    }

    CPoint3D(const float* vec)
    {
        Zero();
        x = vec[0];
        y = vec[1];
        z = vec[2];
    }

    // for colors
    CPoint3D(float a, float b, float c, float d)
    {
        Zero();
        x = a;
        y = b;
        z = c;
        alpha = d;
    }

    inline void SetUV(float newU, float newV)
    {
        u = newU;
        v = newV;
    }

    virtual ~CPoint3D()
    {
    };

    void Scale(float s);

    void Divide(float s);

    void Zero();

    void Set(float a, float b, float c)
    {
        x = a;
        y = b;
        z = c;
    }

    void Set(CPoint3D& p)
    {
        x = p.x;
        y = p.y;
        z = p.z;
    }

    bool IsZero()
    {
        return (x == 0 && y == 0 && z == 0);
    }

    void ProjectOnVector(const CPoint3D& vector, const float distance);

    CPoint3D Normalize();

    // check for equality
    bool operator==(const CPoint3D& a) const;

    CPoint3D operator-(const CPoint3D& v) const;

    CPoint3D operator+(const CPoint3D& v) const;

    // multiplication and division by scalar
    CPoint3D operator*(const float a) const;

    // multiplication
    CPoint3D operator*(const CPoint3D& v) const;

    void Negate();

    float Dot(const CPoint3D& p) const;

    float Distance(const CPoint3D& p) const;

    CPoint3D CrossProduct(const CPoint3D& p2) const;

    void ToFloat3(float* val) const
    {
        val[0] = x;
        val[1] = y;
        val[2] = z;
    }

    std::string ToString() const;

};

