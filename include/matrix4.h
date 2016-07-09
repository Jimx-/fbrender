#ifndef _MATRIX4_H_
#define _MATRIX4_H_

#include "types.h"
#include "vector4.h"

#include <iostream>

namespace fbrender {
    
    class Matrix4 {

        friend std::ostream& operator<<(std::ostream&, const Matrix4&);
    private:
        union {
            Real m[4][4];
            Real _m[16];
        };

    public:

        Matrix4()
        {
            for (int i = 0; i < 16; i++) _m[i] = 0;
        }

        Matrix4(Real m00, Real m01, Real m02, Real m03,
                Real m10, Real m11, Real m12, Real m13,
                Real m20, Real m21, Real m22, Real m23,
                Real m30, Real m31, Real m32, Real m33)
        {
            m[0][0] = m00;
            m[0][1] = m01;
            m[0][2] = m02;
            m[0][3] = m03;
            m[1][0] = m10;
            m[1][1] = m11;
            m[1][2] = m12;
            m[1][3] = m13;
            m[2][0] = m20;
            m[2][1] = m21;
            m[2][2] = m22;
            m[2][3] = m23;
            m[3][0] = m30;
            m[3][1] = m31;
            m[3][2] = m32;
            m[3][3] = m33;
        }

        const Real* operator[](size_t i) const
        {
            return m[i];
        }

        Real* operator[](size_t i)
        {
            return m[i];
        }

        Matrix4 operator*(const Matrix4& mat) const
        {
            Matrix4 r;

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    r[j][i] = (m[j][0] * mat.m[0][i]) +
						(m[j][1] * mat.m[1][i]) +
						(m[j][2] * mat.m[2][i]) +
						(m[j][3] * mat.m[3][i]);
                }
            }

            return r;
        }

        Matrix4 transpose() const;
        Matrix4 inverse() const;

        static Matrix4 translate(Real x, Real y, Real z)
        {
            Matrix4 r = IDENTITY;
            r[3][0] = x;
            r[3][1] = y;
            r[3][2] = z;
            return r;
        }

        static Matrix4 scale(Real x, Real y, Real z)
        {
            Matrix4 r = IDENTITY;
            r[0][0] = x;
            r[1][1] = y;
            r[2][2] = z;
            return r;
        }

        static Matrix4 rotate(Real x, Real y, Real z, Real theta)
        {
            Real qsin = (Real)sin(theta * (Real)0.5);
            Real qcos = (Real)cos(theta * (Real)0.5);

            Vector4 vec(x, y, z, 1.0);
            Real w = qcos;
            vec.normalize();

            x = vec.x * qsin;
            y = vec.y * qsin;
            z = vec.z * qsin;

            Matrix4 r;
            r[0][0] = 1 - 2 * y * y - 2 * z * z;
            r[1][0] = 2 * x * y - 2 * w * z;
            r[2][0] = 2 * x * z + 2 * w * y;
            r[0][1] = 2 * x * y + 2 * w * z;
            r[1][1] = 1 - 2 * x * x - 2 * z * z;
            r[2][1] = 2 * y * z - 2 * w * x;
            r[0][2] = 2 * x * z - 2 * w * y;
            r[1][2] = 2 * y * z + 2 * w * x;
            r[2][2] = 1 - 2 * x * x - 2 * y * y;
            r[0][3] = r[1][3] = r[2][3] = 0.0f;
            r[3][0] = r[3][1] = r[3][2] = 0.0f;	
            r[3][3] = 1.0f;

            return r;
        }

        static Matrix4 lookat(const Vector4& pos, const Vector4& at, const Vector4& up)
        {
            Vector4 xa, ya, za;

            za = at - pos;
            za.normalize();
            xa = up.cross_product(za);
            xa.normalize();
            ya = za.cross_product(xa);

            Matrix4 r;
            r[0][0] = xa.x;
            r[1][0] = xa.y;
            r[2][0] = xa.z;
            r[3][0] = -xa.dot_product(pos);

            r[0][1] = ya.x;
            r[1][1] = ya.y;
            r[2][1] = ya.z;
            r[3][1] = -ya.dot_product(pos);

            r[0][2] = za.x;
            r[1][2] = za.y;
            r[2][2] = za.z;
            r[3][2] = -za.dot_product(pos);

            r[0][3] = r[1][3] = r[2][3] = 0.0f;
            r[3][3] = 1.0f;

            return r;
        }

        static Matrix4 perspective(Real fov, Real aspect, Real near, Real far)
        {
            Real fax = 1.0 / (Real)tan(fov * 0.5f);

            Matrix4 r = ZERO;
            r[0][0] = (Real)(fax / aspect);
            r[1][1] = (Real)(fax);
            r[2][2] = far / (far - near);
            r[3][2] = - near * far / (far - near);
            r[2][3] = 1;

            return r;
        }

        static Matrix4 ZERO;
        static Matrix4 IDENTITY;        
    };

}

#endif

