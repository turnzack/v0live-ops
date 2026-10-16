#include "engine/core/GUID.hpp"
#include <random>
#include <sstream>
#include <iomanip>

namespace engine::core {

const GUID GUID::Invalid{0, 0};

GUID::GUID() : m_low(0), m_high(0) {}

GUID::GUID(uint64_t low, uint64_t high) : m_low(low), m_high(high) {}

GUID GUID::generate() {
    static thread_local std::random_device rd;
    static thread_local std::mt19937_64 gen(rd());
    static thread_local std::uniform_int_distribution<uint64_t> dis;

    uint64_t high = dis(gen);
    uint64_t low = dis(gen);

    // Conformance UUID v4 : version 4 (bits 12-15 de time_hi = 0100) et variant 1 (bits 62-63 = 10)
    high = (high & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;
    low = (low & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;

    return GUID(low, high);
}

GUID::GUID(std::string_view uuidString) : m_low(0), m_high(0) {
    if (uuidString.empty()) {
        return;
    }

    std::string clean;
    clean.reserve(32);
    for (char c : uuidString) {
        if (c != '-' && c != '{' && c != '}') {
            clean.push_back(c);
        }
    }

    if (clean.length() == 32) {
        try {
            m_high = std::stoull(clean.substr(0, 16), nullptr, 16);
            m_low  = std::stoull(clean.substr(16, 16), nullptr, 16);
        } catch (...) {
            m_low = 0;
            m_high = 0;
        }
    }
}

std::string GUID::asString() const {
    if (!isValid()) {
        return "00000000-0000-0000-0000-000000000000";
    }

    std::ostringstream oss;
    oss << std::hex << std::setfill('0')
        << std::setw(8)  << static_cast<uint32_t>(m_high >> 32) << "-"
        << std::setw(4)  << static_cast<uint16_t>((m_high >> 16) & 0xFFFF) << "-"
        << std::setw(4)  << static_cast<uint16_t>(m_high & 0xFFFF) << "-"
        << std::setw(4)  << static_cast<uint16_t>(m_low >> 48) << "-"
        << std::setw(12) << (m_low & 0xFFFFFFFFFFFFULL);
    return oss.str();
}

} // namespace engine::core
