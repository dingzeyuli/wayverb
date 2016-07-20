#include "common/aligned_allocator.h"

#include "gtest/gtest.h"

#include <array>
#include <map>
#include <set>
#include <vector>

namespace {

struct alignas(1 << 4) troublesome {
    float s;
};

struct alignas(1 << 5) nested {
    float s;
    troublesome t;
};

constexpr bool operator<(const nested& a, const nested& b) { return a.s < b.s; }

}  // namespace

int main() {
    {
        using vec_type =
                std::vector<troublesome, mem::aligned_allocator<troublesome>>;
        vec_type a{{0.1f}, {0.2f}, {0.3f}, {0.4f}};
        vec_type b{{0.5f}, {0.6f}, {0.7f}, {0.8f}};
        a = std::move(b);
        for (const auto& i : a) {
            std::cout << i.s << '\n';
        }
        std::cout << std::flush;
    }

    {
        using map_type = std::map<
                int,
                troublesome,
                std::less<int>,
                mem::aligned_allocator<std::pair<const int, troublesome>>>;
        map_type a{{1, {0.1f}}, {2, {0.2f}}, {3, {0.3f}}};
        map_type b{{4, {0.4f}}, {5, {0.5f}}, {6, {0.6f}}};
        a = std::move(b);
        for (const auto& i : a) {
            std::cout << i.second.s << '\n';
        }
        std::cout << std::flush;
    }

    {
        using set_type = std::
                set<nested, std::less<nested>, mem::aligned_allocator<nested>>;
        set_type a{{0.1f, {10.0f}}, {0.2f, {20.0f}}, {0.3f, {30.0f}}};
        set_type b{{0.4f, {40.0f}}, {0.5f, {50.0f}}, {0.6f, {60.0f}}};
        a = std::move(b);
        for (const auto& i : a) {
            std::cout << i.t.s << '\n';
        }
        std::cout << std::flush;
    }

    {
        using arr_type = std::array<nested, 3>;
        arr_type a{{{0.1f, {10.0f}}, {0.2f, {20.0f}}, {0.3f, {30.0f}}}};
        arr_type b{{{0.4f, {40.0f}}, {0.5f, {50.0f}}, {0.6f, {60.0f}}}};

        a = std::move(b);
        for (const auto& i : a) {
            std::cout << i.t.s << '\n';
        }
        std::cout << std::flush;
    }
}