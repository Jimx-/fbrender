#include "vector4.h"
#include "matrix4.h"

namespace fbrender {
    
    Vector4 Vector4::ZERO(0.0, 0.0, 0.0, 0.0);

    Vector4 operator*(const Vector4& v, const Matrix4& m)
    {
        return Vector4(
            v.x*m[0][0] + v.y*m[1][0] + v.z*m[2][0] + v.w*m[3][0],
            v.x*m[0][1] + v.y*m[1][1] + v.z*m[2][1] + v.w*m[3][1],
            v.x*m[0][2] + v.y*m[1][2] + v.z*m[2][2] + v.w*m[3][2],
            v.x*m[0][3] + v.y*m[1][3] + v.z*m[2][3] + v.w*m[3][3]
            );
    }

    std::ostream& operator<<(std::ostream& os, const Vector4& v)
    {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
        return os;
    }
}

