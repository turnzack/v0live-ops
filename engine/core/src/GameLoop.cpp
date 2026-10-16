#include "engine/core/GameLoop.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

namespace engine {

GameLoop::GameLoop(
    double fixedDelta,
    int maxStepsPerFrame,
    double maxFrameDelta
) : fixedDelta_(fixedDelta),
    maxStepsPerFrame_(maxStepsPerFrame),
    maxFrameDelta_(maxFrameDelta) {
}

double GameLoop::interpolationAlpha() const noexcept {
    return alpha_;
}

void GameLoop::run(
    const std::function<bool()>& shouldContinue,
    const UpdateCallback& fixedUpdate,
    const RenderCallback& render
) {
    using clock = std::chrono::steady_clock;
    auto previous = clock::now();

    while (shouldContinue()) {
        const auto now = clock::now();
        double frameDelta = std::chrono::duration<double>(now - previous).count();
        previous = now;

        frameDelta = std::min(frameDelta, maxFrameDelta_);
        accumulator_ += frameDelta;

        int steps = 0;
        while (accumulator_ >= fixedDelta_ && steps < maxStepsPerFrame_) {
            fixedUpdate(fixedDelta_);
            accumulator_ -= fixedDelta_;
            ++steps;
        }

        if (steps == maxStepsPerFrame_ && accumulator_ >= fixedDelta_) {
            spdlog::warn(
                "Frame budget exceeded: dropping {:.3f}s of simulation time",
                accumulator_
            );
            accumulator_ = 0.0;
        }

        alpha_ = accumulator_ / fixedDelta_;
        render(alpha_);
    }
}

} // namespace engine
