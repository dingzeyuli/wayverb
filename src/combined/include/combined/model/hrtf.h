#pragma once

#include "combined/model/member.h"

#include "core/attenuator/hrtf.h"
#include "core/serialize/attenuators.h"

namespace wayverb {
namespace combined {
namespace model {

class hrtf final : public member<hrtf> {
public:
    hrtf() = default;

    void set_orientation(float azimuth, float elevation);
    void set_channel(core::attenuator::hrtf::channel channel);

    core::attenuator::hrtf get() const;

    template <typename Archive>
    void load(Archive& archive) {
        archive(hrtf_);
        notify();
    }

    template <typename Archive>
    void save(Archive& archive) const {
        archive(hrtf_);
    }

private:
    core::attenuator::hrtf hrtf_;
};

}  // namespace model
}  // namespace combined
}  // namespace wayverb
