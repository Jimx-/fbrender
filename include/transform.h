#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "matrix4.h"
#include "vertex.h"

namespace fbrender {

    class Transform {
    public:
        Transform() { }

        Transform(Real width, Real height);

        Vertex apply_mv_transform(const Vertex& v);
        Vertex apply_projection(const Vertex& v);

        const Matrix4& get_world() const { return world; }
        const Matrix4& get_view() const { return view; }
        const Matrix4& get_projection() const { return projection; }

        void set_world(const Matrix4& m)
        {
            world = m;
            update();
        }

        void set_view(const Matrix4& m)
        {
            view = m;
            update();
        }

        void set_projection(const Matrix4& m)
        {
            projection = m;
        }

        Vertex homogenize(const Vertex& v);

        static int check_cvv(const Vertex& v);
        
    private:
        Matrix4 world, view, projection, transform;
        Real height, width;

        void update();
    };

}

#endif

