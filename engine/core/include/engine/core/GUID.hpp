#pragma once

#include <string>
#include <string_view>
#include <cstdint>
#include <functional>

namespace engine::core {

class GUID {
public:
    GUID();
    explicit GUID(uint64_t low, uint64_t high);
    explicit GUID(std::string_view uuidString);

    static GUID generate();
    static const GUID Invalid;

    [[nodiscard]] bool isValid() const noexcept { return m_low != 0 || m_high != 0; }
    [[nodiscard]] std::string asString() const;

    bool operator==(const GUID& rhs) const noexcept { return m_low == rhs.m_low && m_high == rhs.m_high; }
    bool operator!=(const GUID& rhs) const noexcept { return !(*this == rhs); }
    bool operator<(const GUID& rhs) const noexcept {
        return (m_high < rhs.m_high) || (m_high == rhs.m_high && m_low < rhs.m_low);
    }

    [[nodiscard]] uint64_t getLow() const noexcept { return m_low; }
    [[nodiscard]] uint64_t getHigh() const noexcept { return m_high; }

private:
    uint64_t m_low{0};
    uint64_t m_high{0};
};

} // namespace engine::core

namespace std {
    template <>
    struct hash<engine::core::GUID> {
        size_t operator()(const engine::core::GUID& guid) const noexcept {
            return std::hash<uint64_t>{}(guid.getLow()) ^ (std::hash<uint64_t>{}(guid.getHigh()) << 1);
        }
    };
}
