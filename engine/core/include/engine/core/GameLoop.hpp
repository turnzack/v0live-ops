#pragma once

#include <chrono>
#include <functional>

namespace engine {

class GameLoop {
public:
    using UpdateCallback = std::function<void(double fixedDelta)>;
    using RenderCallback = std::function<void(double alpha)>;

    GameLoop(double fixedDelta, int maxStepsPerFrame, double maxFrameDelta);

    void run(
        const std::function<bool()>& shouldContinue,
        const UpdateCallback& fixedUpdate,
        const RenderCallback& render
    );

    [[nodiscard]] double interpolationAlpha() const noexcept;

private:
    double fixedDelta_;
    int maxStepsPerFrame_;
    double maxFrameDelta_;
    double accumulator_ = 0.0;
    double alpha_ = 0.0;
};

} // namespace engine
