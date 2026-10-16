#pragma once

#include <string>
#include <string_view>
#include <cstddef>
#include <cstdint>

namespace engine::core {

class SHA256 {
public:
    static std::string hashString(std::string_view input);
    static std::string hashBytes(const void* data, std::size_t length);
    static std::string hashFile(std::string_view filePath);
};

} // namespace engine::core
