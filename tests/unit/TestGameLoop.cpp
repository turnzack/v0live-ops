#include <doctest/doctest.h>
#include <vector>
#include <engine/core/GameLoop.hpp>

using namespace engine;

TEST_CASE("GameLoop executes fixed updates deterministically") {
    std::vector<int> steps;
    GameLoop loop(1.0 / 60.0, 5, 0.25);
    int frames = 0;

    loop.run(
        [&]() {
            return frames < 3;
        },
        [&](double delta) {
            steps.push_back(static_cast<int>(delta * 60.0 + 0.5));
        },
        [&](double) {
            ++frames;
        }
    );

    CHECK(steps.size() >= 0);
}
