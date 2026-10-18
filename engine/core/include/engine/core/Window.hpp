#pragma once

#include <SDL3/SDL.h>
#include <cstdint>
#include <string>
#include <utility>
#include "engine/core/EngineConfig.hpp"

namespace engine {

class Window {
public:
    explicit Window(const WindowConfig& config);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    [[nodiscard]] SDL_Window* handle() const noexcept { return window_; }
    [[nodiscard]] std::pair<int, int> size() const noexcept;
    [[nodiscard]] bool shouldClose() const noexcept { return shouldClose_; }
    [[nodiscard]] bool isMinimized() const noexcept {
        return window_ && (SDL_GetWindowFlags(window_) & SDL_WINDOW_MINIMIZED);
    }
    void requestClose() noexcept { shouldClose_ = true; }
    void pollEvents();

private:
    SDL_Window* window_ = nullptr;
    bool shouldClose_ = false;
};

} // namespace engine
