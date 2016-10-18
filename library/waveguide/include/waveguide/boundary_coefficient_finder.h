#pragma once

#include "waveguide/cl/boundary_index_array.h"
#include "waveguide/cl/structs.h"
#include "waveguide/mesh_descriptor.h"

#include "common/cl/representation.h"
#include "common/program_wrapper.h"
#include "common/spatial_division/scene_buffers.h"

#include "utilities/popcount.h"

namespace waveguide {

constexpr bool is_boundary(cl_int i) {
    return !(i & id_reentrant || i & id_inside);
}

template <size_t dim>
constexpr bool is_boundary(cl_int i) {
    return is_boundary(i) && popcount(i) == dim;
}

constexpr bool is_1d_boundary_or_reentrant(cl_int i) {
    return i == id_reentrant || is_boundary<1>(i);
}

template <typename It, typename Func>
size_t count_boundary_type(It begin, It end, Func f) {
    return std::count_if(
            begin, end, [&](const auto& i) { return f(i.boundary_type); });
}

//  or maybe keep the buffers on the gpu?
struct boundary_index_data final {
    aligned::vector<boundary_index_array_1> b1;
    aligned::vector<boundary_index_array_2> b2;
    aligned::vector<boundary_index_array_3> b3;
};

boundary_index_data compute_boundary_index_data(
        const cl::Device& device,
        const scene_buffers& buffers,
        const mesh_descriptor& descriptor,
        aligned::vector<condensed_node>& nodes);

}  // namespace waveguide