#include "engine/renderer/SpriteBatch.hpp"

#include <cmath>

namespace engine {

void SpriteBatch::begin() {
    vertices_.clear();
    indices_.clear();
    spriteCount_ = 0;
}

void SpriteBatch::draw(const SpriteDrawCommand& command) {
    const glm::mat2 rotation{
        std::cos(command.rotation), -std::sin(command.rotation),
        std::sin(command.rotation),  std::cos(command.rotation)
    };

    const glm::vec2 halfSize = command.size * 0.5f;

    const glm::vec2 corners[4] = {
        {-halfSize.x, -halfSize.y},
        { halfSize.x, -halfSize.y},
        { halfSize.x,  halfSize.y},
        {-halfSize.x,  halfSize.y}
    };

    const glm::vec2 uvs[4] = {
        {command.uvRect.x, command.uvRect.w},
        {command.uvRect.z, command.uvRect.w},
        {command.uvRect.z, command.uvRect.y},
        {command.uvRect.x, command.uvRect.y}
    };

    const std::uint32_t base = static_cast<std::uint32_t>(vertices_.size());

    for (int i = 0; i < 4; ++i) {
        vertices_.push_back({
            command.position + rotation * corners[i],
            uvs[i],
            command.color
        });
    }

    indices_.insert(indices_.end(), {
        base + 0, base + 1, base + 2,
        base + 0, base + 2, base + 3
    });

    ++spriteCount_;
}

void SpriteBatch::end() {
}

const std::vector<SpriteVertex>& SpriteBatch::vertices() const noexcept {
    return vertices_;
}

const std::vector<std::uint32_t>& SpriteBatch::indices() const noexcept {
    return indices_;
}

std::size_t SpriteBatch::spriteCount() const noexcept {
    return spriteCount_;
}

} // namespace engine
