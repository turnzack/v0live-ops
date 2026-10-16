#include <doctest/doctest.h>

#include <SDL3/SDL.h>

#include "engine/input/InputManager.hpp"

using namespace engine;

TEST_CASE("InputManager tracks pressed, held and released states") {
    InputManager input;
    input.bindKey(SDLK_RIGHT, InputAction::MoveRight);

    SDL_Event down{};
    down.type = SDL_EVENT_KEY_DOWN;
    down.key.key = SDLK_RIGHT;

    SDL_Event up{};
    up.type = SDL_EVENT_KEY_UP;
    up.key.key = SDLK_RIGHT;

    input.beginFrame();
    input.handleEvent(down);

    CHECK(input.isDown(InputAction::MoveRight));
    CHECK(input.wasPressed(InputAction::MoveRight));

    input.beginFrame();

    CHECK(input.isDown(InputAction::MoveRight));
    CHECK_FALSE(input.wasPressed(InputAction::MoveRight));

    input.handleEvent(up);

    CHECK_FALSE(input.isDown(InputAction::MoveRight));
    CHECK(input.wasReleased(InputAction::MoveRight));
}

TEST_CASE("InputManager normalizes diagonal movement") {
    InputManager input;

    input.bindKey(SDLK_RIGHT, InputAction::MoveRight);
    input.bindKey(SDLK_DOWN, InputAction::MoveDown);

    SDL_Event right{};
    right.type = SDL_EVENT_KEY_DOWN;
    right.key.key = SDLK_RIGHT;

    SDL_Event down{};
    down.type = SDL_EVENT_KEY_DOWN;
    down.key.key = SDLK_DOWN;

    input.beginFrame();
    input.handleEvent(right);
    input.handleEvent(down);

    const glm::vec2 axis = input.movementAxis();

    CHECK(axis.x == doctest::Approx(0.70710678f));
    CHECK(axis.y == doctest::Approx(0.70710678f));
}
