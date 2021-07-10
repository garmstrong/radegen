#include <sstream>
#include "point3d.h"

namespace rade
{
    void vector3::Scale(float s)
    {
        x *= s;
        y *= s;
        z *= s;
    }

    void vector3::Divide(float s)
    {
        x /= s;
        y /= s;
        z /= s;
    }

    void vector3::Zero()
    {
        x = 0;
        y = 0;
        z = 0;
        u = 0;
        v = 0;
        nx = 0;
        ny = 0;
        nz = 0;
        useNormal = false;
    }

    void vector3::ProjectOnVector(const rade::vector3& vector, const float distance)
    {
        // project a point from the camera in the view dir
        x = (vector.x * distance) + x;
        y = (vector.y * distance) + y;
        z = (vector.z * distance) + z;
    }

    // normalise
    vector3 vector3::Normalize()
    {
        float length;
        length = x * x + y * y + z * z;
        length = static_cast<float>(sqrtf(length));
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
    bool vector3::operator==(const vector3& a) const
    {
        return (fabsf(x - a.x) < rade::math::cEpsilon) && (fabsf(y - a.y) < rade::math::cEpsilon) &&
               (fabsf(z - a.z) < rade::math::cEpsilon);
    }

    // subtract
    vector3 vector3::operator-(const vector3& vec) const
    {
        vector3 newPos;
        newPos = *this;
        newPos.Set(x - vec.x,
                y - vec.y,
                z - vec.z);
        return newPos;
    }

    // add
    vector3 vector3::operator+(const vector3& vec) const
    {
        vector3 newPos;
        newPos = *this;
        newPos.Set(x + vec.x,
                y + vec.y,
                z + vec.z);
        return newPos;
    }

    // multiplication and division by scalar
    vector3 vector3::operator*(const float a) const
    {
        return {x * a,
                y * a,
                z * a};
    }

    // multiplication
    vector3 vector3::operator*(const rade::vector3& vec) const
    {
        return {x * vec.x,
                y * vec.y,
                z * vec.z};
    }

    // invert
    void vector3::Negate()
    {
        x = -x;
        y = -y;
        z = -z;
    }

    float vector3::Dot(const vector3& p) const
    {
        return x * p.x + y * p.y + z * p.z;
    }

    // distance between two points
    float vector3::Distance(const vector3& p) const
    {
        return sqrtf(rade::math::vsqr(fabsf(p.x - x)) + rade::math::vsqr(fabsf(p.y - y)) + rade::math::vsqr(fabsf(p.z - z)));
        /*vector3 r;
        r.x = x - p.x;
        r.y = y - p.y;
        r.z = z - p.z;
        return sqrt(r.x*r.x + r.y*r.y + r.z*r.z);*/
    }

    // cross product
    vector3 vector3::CrossProduct(const vector3& p2) const
    {
        return {
                y * p2.z - z * p2.y,
                z * p2.x - x * p2.z,
                x * p2.y - y * p2.x};
    }

    // to string, for debugging
    std::string vector3::ToString() const
    {
        std::ostringstream pointString;
        pointString << "(" << x << ", " << y << ", " << z << ")";
        return pointString.str();
    }

};
