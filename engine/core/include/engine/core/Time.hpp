#pragma once

#include <chrono>
#include <cstdint>

namespace engine::core {

class Time {
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using DurationSeconds = std::chrono::duration<float>;

    Time(float fixedTimestepHz = 60.0f, float maxAccumulatedSeconds = 0.2f);

    void reset();
    void tick();

    // Retourne true s'il reste au moins un pas fixe à simuler dans l'accumulateur
    [[nodiscard]] bool shouldDoFixedStep();

    [[nodiscard]] float getDeltaTime() const noexcept { return m_deltaTime; }
    [[nodiscard]] float getTotalTime() const noexcept { return m_totalTime; }
    [[nodiscard]] float getFixedDeltaTime() const noexcept { return m_fixedDeltaTime; }
    [[nodiscard]] float getAlpha() const noexcept { return m_alpha; }
    [[nodiscard]] float getFPS() const noexcept { return m_fps; }
    [[nodiscard]] uint64_t getFrameCount() const noexcept { return m_frameCount; }

    void setFixedTimestepHz(float hz) noexcept;

private:
    TimePoint m_startTime;
    TimePoint m_lastFrameTime;
    TimePoint m_lastFpsTime;

    float m_deltaTime{0.0f};
    float m_totalTime{0.0f};
    float m_fixedDeltaTime{1.0f / 60.0f};
    float m_accumulator{0.0f};
    float m_maxAccumulatedSeconds{0.2f};
    float m_alpha{0.0f};

    float m_fps{0.0f};
    uint32_t m_fpsFrames{0};
    uint64_t m_frameCount{0};
};

} // namespace engine::core
