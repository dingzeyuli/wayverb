#include "waveguide/make_transparent.h"
#include "waveguide/mesh.h"
#include "waveguide/postprocessor/node.h"
#include "waveguide/preprocessor/soft_source.h"
#include "waveguide/waveguide.h"

#include "compensation_signal/waveguide.h"

#include "common/callback_accumulator.h"
#include "common/spatial_division/voxelised_scene_data.h"

#include "utilities/progress_bar.h"

#include "gtest/gtest.h"

#include <cassert>
#include <cmath>

namespace {
template <typename T>
void multitest(T run) {
    constexpr auto iterations{100};
    const auto proper_output{run()};
    for (auto i{0ul}; i != iterations; ++i) {
        const auto output{run()};
        ASSERT_EQ(output, proper_output);
    }
}
}  // namespace

TEST(verify_compensation_signal, verify_compensation_signal_compressed) {
    const std::vector<float> input{1, 2, 3, 4, 5, 4, 3, 2, 1};
    const auto transparent{waveguide::make_transparent(
            input.data(), input.data() + input.size())};

    const auto steps{100};

    const compute_context c{};
    compressed_rectangular_waveguide waveguide(c, steps);

    multitest([&] {
        auto t{transparent};
        progress_bar pb{std::cerr};
        return waveguide.run_soft_source(t.begin(), t.end(), [&](auto step) {
            set_progress(pb, step, steps);
        });
    });
}

TEST(verify_compensation_signal, verify_compensation_signal_normal) {
    const std::vector<float> input{1, 2, 3, 4, 5, 6, 5, 4, 3, 2, 1};
    auto transparent{waveguide::make_transparent(input.data(),
                                                 input.data() + input.size())};
    transparent.resize(100);

    const compute_context cc{};

    auto scene_data{geo::get_scene_data(geo::box(glm::vec3(-1), glm::vec3(1)),
                                        make_surface(0.5, 0))};
    const auto voxelised{make_voxelised_scene_data(
            scene_data, 5, padded(geo::get_aabb(scene_data), glm::vec3{0.1}))};

    constexpr auto speed_of_sound{340.0};
    const auto model{
            waveguide::compute_mesh(cc, voxelised, 0.05, speed_of_sound)};

    constexpr glm::vec3 centre{0, 0, 0};
    const auto receiver_index{compute_index(model.get_descriptor(), centre)};

    multitest([&] {
        auto prep{waveguide::preprocessor::make_soft_source(
                receiver_index, transparent.begin(), transparent.end())};

        callback_accumulator<waveguide::postprocessor::node> postprocessor{
                receiver_index};

        progress_bar pb{std::cout};
        waveguide::run(cc,
                       model,
                       prep,
                       [&](auto& queue, const auto& buffer, auto step) {
                           postprocessor(queue, buffer, step);
                           set_progress(pb, step, transparent.size());
                       },
                       true);

        assert(postprocessor.get_output().size() == transparent.size());

        return postprocessor.get_output();
    });
}