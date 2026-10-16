#pragma once

#include <webgpu/webgpu_cpp.h>
#include <vector>
#include <cmath>
#include <cstdint>
#include <glm/glm.hpp>

namespace engine {

struct SpriteVertex {
    glm::vec2 position;
    glm::vec2 uv;
    glm::vec4 color;
};

struct SpriteDrawCommand {
    glm::vec2 position;
    glm::vec2 size;
    float rotation = 0.0f;
    glm::vec4 color = glm::vec4(1.0f);
    glm::vec4 uvRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
};

class SpriteBatch {
public:
    void begin();
    void draw(const SpriteDrawCommand& command);
    void end();

    [[nodiscard]] const std::vector<SpriteVertex>& vertices() const noexcept;
    [[nodiscard]] const std::vector<std::uint32_t>& indices() const noexcept;
    [[nodiscard]] std::size_t spriteCount() const noexcept;

private:
    std::vector<SpriteVertex> vertices_;
    std::vector<std::uint32_t> indices_;
    std::size_t spriteCount_ = 0;
};

} // namespace engine
