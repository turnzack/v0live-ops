#pragma once

#include <glm/glm.hpp>

namespace engine {

class Camera2D {
public:
    void follow(glm::vec2 target, float fixedDelta);
    void setZoom(float zoom);
    void setDeadZone(float deadZone);
    [[nodiscard]] glm::mat4 viewProjection(
        float viewportWidth, float viewportHeight
    ) const;
    [[nodiscard]] glm::vec2 position() const noexcept { return position_; }
    [[nodiscard]] float zoom() const noexcept { return zoom_; }

private:
    glm::vec2 position_{0.0f, 0.0f};
    float zoom_ = 1.0f;
    float deadZone_ = 32.0f;
    float smoothing_ = 8.0f;
};

} // namespace engine
