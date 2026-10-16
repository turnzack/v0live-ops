#include "engine/renderer/Camera2D.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

namespace engine {

void Camera2D::follow(glm::vec2 target, float fixedDelta) {
    const glm::vec2 delta = target - position_;
    if (std::abs(delta.x) > deadZone_) {
        position_.x += delta.x * std::min(smoothing_ * fixedDelta, 1.0f);
    }
    if (std::abs(delta.y) > deadZone_) {
        position_.y += delta.y * std::min(smoothing_ * fixedDelta, 1.0f);
    }
}

void Camera2D::setZoom(float zoom) {
    zoom_ = std::clamp(zoom, 0.1f, 10.0f);
}

void Camera2D::setDeadZone(float deadZone) {
    deadZone_ = std::max(0.0f, deadZone);
}

glm::mat4 Camera2D::viewProjection(
    float viewportWidth, float viewportHeight
) const {
    const float halfWidth = viewportWidth * 0.5f / zoom_;
    const float halfHeight = viewportHeight * 0.5f / zoom_;
    const glm::mat4 projection = glm::ortho(
        -halfWidth, halfWidth, -halfHeight, halfHeight, -100.0f, 100.0f
    );
    const glm::mat4 view = glm::translate(
        glm::mat4(1.0f), glm::vec3(-position_, 0.0f)
    );
    return projection * view;
}

} // namespace engine
