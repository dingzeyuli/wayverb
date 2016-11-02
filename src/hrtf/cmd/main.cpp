/// A program to read a bunch of HRTF kernels and create a simple C++ header
/// file containing an array of hrtf::entry structs.
/// These can be processed at run-time to produce a fast lookup table (or a
/// slow lookup table or whatever).

#include "bracketer.h"
#include "dir.h"

#include "hrtf/multiband.h"

#include "utilities/map.h"

#include "frequency_domain/multiband_filter.h"

#include "audio_file/audio_file.h"

#include <array>
#include <experimental/optional>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <regex>

template <typename T, size_t I>
std::ostream& operator<<(std::ostream& os, const std::array<T, I>& arr) {
    bracketer b{os, "{{", "}}"};
    for (const auto& item : arr) {
        os << item << ",\n";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const hrtf_data::entry& x) {
    bracketer b{os, "{", "}"};
    return os << x.azimuth << ",\n"
              << x.elevation << ",\n"
              << x.energy << ",\n";
}

template <typename T, typename Alloc>
std::ostream& operator<<(std::ostream& os, const std::vector<T, Alloc>& t) {
    bracketer b{os, "{", "}"};
    for (const auto& item : t) {
        os << item << ",\n";
    }
    return os;
}

void generate_data_file(std::ostream& os,
                        const std::vector<hrtf_data::entry>& entries) {
    os << std::setprecision(std::numeric_limits<double>::max_digits10);
    os << R"(
//  Autogenerated file //
#pragma once
#include "hrtf/entry.h"
namespace hrtf_data {
constexpr entry entries[] = )"
       << entries << R"(;
}  // namespace hrtf_data
)";
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "expected a directory name\n";
        return EXIT_FAILURE;
    }

    const std::string base_path{argv[1]};

    std::vector<hrtf_data::entry> results;

    const auto entries = list_directory(base_path.c_str());
    const std::regex name_regex{".*R([0-9]+)_T([0-9]+)_P([0-9]+).*"};
    for (const auto& entry : entries) {
        std::smatch match{};
        if (std::regex_match(entry, match, name_regex)) {
            const auto az = std::stoi(match[2].str());
            const auto el = std::stoi(match[3].str());

            const auto full_path = base_path + "/" + entry;

            const auto audio=audio_file::read(full_path);

            if (audio.signal.size() != 2) {
                throw std::runtime_error{"hrtf data files must be stereo"};
            }

            const auto energy = util::map(
                    [&](auto channel) {
                        return util::map([](auto i) { return sqrt(i); },
                                         hrtf_data::per_band_energy(
                                                 begin(audio.signal[channel]),
                                                 end(audio.signal[channel]),
                                                 audio.sample_rate));
                    },
                    std::array<size_t, 2>{{0, 1}});

            results.emplace_back(hrtf_data::entry{az, el, energy});
        }
    }

    generate_data_file(std::cout, results);

    return EXIT_SUCCESS;
}
