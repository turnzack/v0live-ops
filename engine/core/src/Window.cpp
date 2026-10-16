#include "engine/core/Window.hpp"
#include <spdlog/spdlog.h>
#include "engine/core/Assert.hpp"

namespace engine {

Window::Window(const WindowConfig& config) {
    ENGINE_VERIFY(SDL_Init(SDL_INIT_VIDEO), "SDL_Init(SDL_INIT_VIDEO) failed: {}", SDL_GetError());

    SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    window_ = SDL_CreateWindow(
        config.title.c_str(),
        config.width,
        config.height,
        flags
    );
    ENGINE_VERIFY(window_ != nullptr, "SDL_CreateWindow failed: {}", SDL_GetError());

    spdlog::info(
        "Window created: {} ({}x{})",
        config.title, config.width, config.height
    );
}

Window::~Window() {
    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    SDL_Quit();
}

std::pair<int, int> Window::size() const noexcept {
    int width = 0;
    int height = 0;
    SDL_GetWindowSize(window_, &width, &height);
    return {width, height};
}

void Window::pollEvents() {
    SDL_Event event{};
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                requestClose();
                break;
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                requestClose();
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                spdlog::debug(
                    "Window resized: {}x{}",
                    event.window.data1, event.window.data2
                );
                break;
            default:
                break;
        }
    }
}

} // namespace engine
