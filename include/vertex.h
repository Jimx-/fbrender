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
        Vector4 _pos;
        TexCoord _tex;
        Vector4 _normal;
        Vector4 _world_pos;
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

        Vertex(const Vector4& pos, const TexCoord& tex, const Color& color, const Vector4& world_pos, const Vector4& normal)
            : _pos(pos), _tex(tex), _color(color), _world_pos(world_pos), _normal(normal)
        {
            if (_pos.w != 0.0) invw = 1.0 / _pos.w;
        }

        const Color& get_color() const { return _color; }
        const Vector4& get_pos() const { return _pos; }
        const TexCoord& get_texcoord() const { return _tex; }
        const Vector4& get_normal() const { return _normal; }
        const Vector4& get_world_pos() const { return _world_pos; }
        void set_normal(const Vector4& n) { _normal = n; }
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

            _world_pos.x = LERP(v1._world_pos.x, v2._world_pos.x, t);
            _world_pos.y = LERP(v1._world_pos.y, v2._world_pos.y, t);
            _world_pos.z = LERP(v1._world_pos.z, v2._world_pos.z, t);

            _normal.x = LERP(v1._normal.x, v2._normal.x, t);
            _normal.y = LERP(v1._normal.y, v2._normal.y, t);
            _normal.z = LERP(v1._normal.z, v2._normal.z, t);

            invw = LERP(v1.invw, v2.invw, t);
#undef LERP
        }
    };
}

#endif

