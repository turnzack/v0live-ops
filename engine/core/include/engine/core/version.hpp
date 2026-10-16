#pragma once

#include <string_view>

namespace engine::core {

struct Version {
    int major{0};
    int minor{1};
    int patch{0};

    [[nodiscard]] static constexpr std::string_view asString() noexcept {
        return "0.1.0-alpha";
    }
};

} // namespace engine::core
