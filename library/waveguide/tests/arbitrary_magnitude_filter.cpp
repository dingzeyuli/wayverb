#include "waveguide/arbitrary_magnitude_filter.h"
#include "waveguide/stable.h"

#include "gtest/gtest.h"

#include <random>

TEST(arbitrary_magnitude_filter, stable) {
    const auto test = [](const auto& env) {
        ASSERT_TRUE(waveguide::is_stable(
                waveguide::arbitrary_magnitude_filter<6>(env).a));
    };

    test(waveguide::frequency_domain_envelope{});

    {
        auto env = waveguide::frequency_domain_envelope{};

        env.insert(waveguide::frequency_domain_envelope::point{0, 0});
        test(env);

        env.insert(waveguide::frequency_domain_envelope::point{0.5, 1});
        test(env);

        env.insert(waveguide::frequency_domain_envelope::point{0.49, 0});
        test(env);

        env.insert(waveguide::frequency_domain_envelope::point{0.51, 0});
        test(env);
    }

    auto engine = std::default_random_engine{std::random_device{}()};
    auto dist = std::uniform_real_distribution<float>{0, 1};

    for (auto i = 0; i != 1000; ++i) {
        auto env = waveguide::frequency_domain_envelope{};
        for (auto j = 0; j != 100; ++j) {
            env.insert(waveguide::frequency_domain_envelope::point{
                    dist(engine), dist(engine)});
        }
        test(env);
    }
}
