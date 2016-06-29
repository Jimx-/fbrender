#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "vector4.h"

namespace fbrender {

    struct Color {
        Real r, g, b;

        Color(Real r, Real g, Real b) : r(r), g(b), b(b) { }

        Color operator*(Real f) const
        {
            return Color(r * f, g * f, b * f);
        }

        Color& operator*=(Real f)
        {
            *this = *this * f;
            return *this;
        }

        uint32_t color_value() const
        {
            uint32_t ri = (int)(r * 255);
            uint32_t gi = (int)(g * 255);
            uint32_t bi = (int)(b * 255);

            return (ri << 16) | (gi << 8) | (bi);
        }
    };

    struct TexCoord {
        Real u, v;

        TexCoord(Real u, Real v) : u(u), v(v) { }

        TexCoord operator*(Real f) const
        {
            return TexCoord(u * f, v * f);
        }

        TexCoord& operator*=(Real f)
        {
            *this = *this * f;
            return *this;
        }
    };


    class Vertex {
    private:
        Color _color;
        Vector4 _pos;
        TexCoord _tex;
        Real invw;

    public:
        Vertex(Real x, Real y, Real z, Real w, Real u, Real v, Real r, Real g, Real b) 
            : _pos(x, y, z, w), _color(r, g, b), _tex(u, v)
        {
            if (w != 0.0) invw = 1.0 / w;
        }

        Vertex(const Vector4& pos, const TexCoord& tex, const Color& color)
            : _pos(pos), _tex(tex), _color(color)
        {
            if (_pos.w != 0.0) invw = 1.0 / _pos.w;
        }

        const Color& get_color() const { return _color; }
        const Vector4& get_pos() const { return _pos; }
        const TexCoord& get_texcoord() const { return _tex; }
        Real get_one_per_w() const { return invw; }
        void set_one_per_w(Real inv) { invw = inv; }

        void lerp(const Vertex& v1, const Vertex& v2, Real t)
        {
#define LERP(a, b, t) (b * t + a * (1 - t))
            _tex.u = LERP(v1._tex.u, v2._tex.u, t);
            _tex.v = LERP(v1._tex.v, v2._tex.v, t);

            _color.r = LERP(v1._color.r, v2._color.r, t);
            _color.g = LERP(v1._color.g, v2._color.g, t);
            _color.b = LERP(v1._color.b, v2._color.b, t);

            invw = LERP(v1.invw, v2.invw, t);
#undef LERP
        }
    };
}

#endif

