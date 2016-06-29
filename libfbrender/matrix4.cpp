#include "matrix4.h"
#include <iomanip>
using std::ostream;

namespace fbrender {

    Matrix4 Matrix4::ZERO(0, 0, 0, 0,
                                0, 0, 0, 0,
                                0, 0, 0, 0,
                                0, 0, 0, 0);

    Matrix4 Matrix4::IDENTITY(1, 0, 0, 0,
                                0, 1, 0, 0,
                                0, 0, 1, 0,
                                0, 0, 0, 1);


    ostream& operator<<(ostream& os, const Matrix4& mat)
    {
        os << "{";
        for (int i = 0; i < 4; i++) {
            if (i) os << " ";
            os << "{";
            for (int j = 0; j < 4; j++) { 
                if (j) os << ", ";
                os << std::setw(6) << mat[i][j];
            }
            os << "}";
            if (i != 3) os << std::endl;
        }
        os << "}";
        return os;
    }

}

