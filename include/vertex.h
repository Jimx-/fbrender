#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "vector4.h"

namespace fbrender {

    struct Color {
        Real r, g, b;

#define CLAMP(x, a, b) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))
        Color(Real r = 0.0, Real g = 0.0, Real b = 0.0)
        {
            this->r = CLAMP(r, 0, 1);
            this->g = CLAMP(g, 0, 1);
            this->b = CLAMP(b, 0, 1);
        }

        Color(uint32_t val)
        {
            int rc = (val >> 16) & 0xff;
            int gc = (val >> 8) & 0xff;
            int bc = (val) & 0xff;

            this->r = CLAMP(rc / 255.0, 0, 1);
            this->g = CLAMP(gc / 255.0, 0, 1);
            this->b = CLAMP(bc / 255.0, 0, 1);
        }

        Color operator*(Real f) const
        {
            return Color(r * f, g * f, b * f);
        }

        Color operator+(const Color& c) const
        {
            return Color(r + c.r, g + c.g, b + c.b);
        }

        Color operator*(const Color& c) const
        {
            return Color(r * c.r, g * c.g, b * c.b);
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
        Color _lighting_color;
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

        Vertex(const Vector4& pos, const TexCoord& tex, const Color& color, const Color& lighting_color)
            : _pos(pos), _tex(tex), _color(color), _lighting_color(lighting_color)
        {
            if (_pos.w != 0.0) invw = 1.0 / _pos.w;
        }

        const Color& get_color() const { return _color; }
        const Vector4& get_pos() const { return _pos; }
        const TexCoord& get_texcoord() const { return _tex; }
        void set_lighting_color(const Color& color) { _lighting_color = color; }
        const Color& get_lighting_color() const { return _lighting_color; }
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

            _lighting_color.r = LERP(v1._lighting_color.r, v2._lighting_color.r, t);
            _lighting_color.g = LERP(v1._lighting_color.g, v2._lighting_color.g, t);
            _lighting_color.b = LERP(v1._lighting_color.b, v2._lighting_color.b, t);

            invw = LERP(v1.invw, v2.invw, t);
#undef LERP
        }
    };
}

#endif

