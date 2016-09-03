#ifndef _VECTOR4_H_
#define _VECTOR4_H_

#include "types.h"

#include <cstddef>
#include <cmath>
#include <iostream>

namespace fbrender {

    class Matrix4;

    struct Vector4 {
    public:
        Real x, y, z, w;

        Vector4(Real x = 0.0, Real y = 0.0, Real z = 0.0, Real w = 0.0)
            : x(x), y(y), z(z), w(w)
        {
        }

        Real operator[](size_t i) const
        {
            return *(&x + i);
        }

        Real& operator[](size_t i)
        {
            return *(&x + i);
        }

        Vector4 operator+(const Vector4& v) const
        {
            return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
        }

        Vector4 operator-(const Vector4& v) const
        {
            return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
        }

        Vector4 operator*(const Vector4& v) const
        {
            return Vector4(x * v.x, y * v.y, z * v.z, w * v.w);
        }

        Vector4 operator*(Real f) const
        {
            return Vector4(x * f, y * f, z * f, w * f);
        }

        Vector4& operator*=(Real f)
        {
            x *= f;
            y *= f;
            z *= f;
            w *= f;
            return *this;
        }

        Real dot_product(const Vector4& v) const
        {
            return x * v.x + y * v.y + z * v.z;
        }

        Vector4 cross_product(const Vector4& v) const
        {
            Real m1, m2, m3;
            m1 = y * v.z - z * v.y;
            m2 = z * v.x - x * v.z;
            m3 = x * v.y - y * v.x;
            return Vector4(m1, m2, m3, 0.0);
        }

        Real length() const
        {
            Real s = x * x + y * y + z * z;
            return (Real) sqrt(s);
        }

        void normalize()
        {
            Real len = length();
            if (len != (Real)0.0) {
                Real inv = 1 / len;
                x *= inv;
                y *= inv;
                z *= inv;
                w *= inv;
            }
        }

        static Vector4 lerp(const Vector4& l, const Vector4& r, float t) 
        {
            return r * t + l * (1 - t);
        }

        static Vector4 ZERO;
    };

    Vector4 operator*(const Vector4& v, const Matrix4& m);
    std::ostream& operator<<(std::ostream& os, const Vector4& v);
}

#endif

