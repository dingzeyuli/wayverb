#include "common/triangle_vec.h"
#include "common/conversions.h"
#include "common/triangle.h"

triangle_vec3 get_triangle_verts(const triangle& t,
                                 const aligned::vector<glm::vec3>& v) {
    return triangle_vec3{{v[t.v0], v[t.v1], v[t.v2]}};
}

triangle_vec3 get_triangle_verts(const triangle& t,
                                 const aligned::vector<cl_float3>& v) {
    return triangle_vec3{
            {to_vec3(v[t.v0]), to_vec3(v[t.v1]), to_vec3(v[t.v2])}};
}
