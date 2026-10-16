#include "engine/core/Time.hpp"
#include <algorithm>

namespace engine::core {

Time::Time(float fixedTimestepHz, float maxAccumulatedSeconds)
    : m_fixedDeltaTime((fixedTimestepHz > 0.0f) ? (1.0f / fixedTimestepHz) : (1.0f / 60.0f))
    , m_maxAccumulatedSeconds(maxAccumulatedSeconds) {
    reset();
}

void Time::reset() {
    m_startTime = Clock::now();
    m_lastFrameTime = m_startTime;
    m_lastFpsTime = m_startTime;
    m_deltaTime = 0.0f;
    m_totalTime = 0.0f;
    m_accumulator = 0.0f;
    m_alpha = 0.0f;
    m_fps = 0.0f;
    m_fpsFrames = 0;
    m_frameCount = 0;
}

void Time::setFixedTimestepHz(float hz) noexcept {
    if (hz > 0.0f) {
        m_fixedDeltaTime = 1.0f / hz;
    }
}

void Time::tick() {
    const auto current = Clock::now();
    m_deltaTime = DurationSeconds(current - m_lastFrameTime).count();
    m_lastFrameTime = current;

    // Protection spiral of death (clamp delta time)
    const float clampedDelta = std::min(m_deltaTime, m_maxAccumulatedSeconds);
    m_accumulator += clampedDelta;
    m_totalTime = DurationSeconds(current - m_startTime).count();

    // Calcul FPS
    m_frameCount++;
    m_fpsFrames++;
    const float fpsInterval = DurationSeconds(current - m_lastFpsTime).count();
    if (fpsInterval >= 1.0f) {
        m_fps = static_cast<float>(m_fpsFrames) / fpsInterval;
        m_fpsFrames = 0;
        m_lastFpsTime = current;
    }

    // Alpha d'interpolation
    m_alpha = (m_fixedDeltaTime > 0.0f) ? (m_accumulator / m_fixedDeltaTime) : 0.0f;
}

bool Time::shouldDoFixedStep() {
    if (m_accumulator >= m_fixedDeltaTime) {
        m_accumulator -= m_fixedDeltaTime;
        m_alpha = (m_fixedDeltaTime > 0.0f) ? (m_accumulator / m_fixedDeltaTime) : 0.0f;
        return true;
    }
    return false;
}

} // namespace engine::core
