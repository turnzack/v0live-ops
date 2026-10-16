#include <doctest/doctest.h>

#include "engine/renderer/SpriteBatch.hpp"

using namespace engine;

TEST_CASE("SpriteBatch creates indexed quads") {
    SpriteBatch batch;

    batch.begin();
    batch.draw({.position = {0.0f, 0.0f}, .size = {64.0f, 64.0f}});
    batch.end();

    CHECK(batch.spriteCount() == 1);
    CHECK(batch.vertices().size() == 4);
    CHECK(batch.indices().size() == 6);
}
