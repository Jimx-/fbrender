#include "transform.h"
#include <iostream>
using namespace std;
namespace fbrender {

    Transform::Transform(Real width, Real height)
    {
        Real aspect = width / height;
        
        world = Matrix4::IDENTITY;
        view = Matrix4::IDENTITY;
        projection = Matrix4::perspective((Real)0.5 * (Real)3.1415926, aspect, 1.0, 500.0);

        this->width = width;
        this->height = height;
        update();
    }

    Vertex Transform::apply_mv_transform(const Vertex& v)
    {
        return Vertex(v.get_pos() * transform, v.get_texcoord(), v.get_color());
    }

    Vertex Transform::apply_projection(const Vertex& v)
    {
        Vector4 pos = v.get_pos() * projection;
        TexCoord tex = v.get_texcoord();
        Color color = v.get_color();
        
        Real invw = 1.0 / pos.w;
        tex *= invw;
        color *= invw;

        return Vertex(pos, tex, color);
    }

    void Transform::update()
    {
        transform = world * view;
    }

    int Transform::check_cvv(const Vertex& v)
    {
        Vector4 vec = v.get_pos();
        Real x = vec.x;
        Real y = vec.y;
        Real z = vec.z;
        Real w = vec.w;
        if (z < 0.0) return 1;
        if (z > w) return 1;
        if (x < -w) return 1;
        if (x > w) return 1;
        if (y < -w) return 1;
        if (y > w) return 1;

        return 0;
    }

    Vertex Transform::homogenize(const Vertex& v)
    {
        Vector4 vec = v.get_pos();
        Real invecw = 1.0 / vec.w;
        Real x = (vec.x * invecw + 1.0) * width * 0.5;
        Real y = (1.0 - vec.y * invecw) * height * 0.5;
        Real z = vec.z * invecw;
        Vertex r(Vector4(x, y, z, 1.0), v.get_texcoord(), v.get_color());
        r.set_one_per_w(invecw);

        return r;
    }

}

